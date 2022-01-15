#include "BCDecoderPlayer.h"
#include <QMutex>
#include <QHoverEvent>
#include <QDebug>
extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavcodec/avfft.h"
    #include "libavformat/avformat.h"
    #include "libavformat/avio.h"
    #include "libswscale/swscale.h"
    #include "libavutil/common.h"
    #include "libavutil/avstring.h"
    #include "libavutil/imgutils.h"
    #include "libavutil/time.h"
    #include "libavdevice/avdevice.h"
    #include "libswresample/swresample.h"

    //#include "SDL2/SDL.h"
    //#include "SDL2/SDL_events.h"
    //#include "SDL2/SDL_thread.h"
}

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logPlayer, "log.player.decoder")


// ---------------------------------------------------------------------------------------------------------------------------
BCDecoderPlayer::BCDecoderPlayer(QWidget *parent) :
    QLabel(parent)
{
    m_VideoPlayer = new VideoPlayer();
    connect(m_VideoPlayer,SIGNAL(sig_GetOneFrame(QImage)),this,SLOT(SetImageSlots(QImage)), Qt::QueuedConnection);
}

BCDecoderPlayer::~BCDecoderPlayer()
{
}


void BCDecoderPlayer::SetImageSlots(const QImage image)
{
    if (image.height() > 0){
        QPixmap pix = QPixmap::fromImage(image);
        this->setPixmap(pix.scaled(this->size()));
    }
}


void BCDecoderPlayer::play(QString url)
{
    m_VideoPlayer->startPlay(url);
}

void BCDecoderPlayer::stop()
{
    m_VideoPlayer->stopPlay();
}

/**
 * @brief VideoPlayer::VideoPlayer
 * @param parent
 */
VideoPlayer::VideoPlayer(QObject *parent)
    : QThread(parent)
{
    avformat_network_init();   //初始化FFmpeg网络模块，2017.8.5---lizhen
    av_register_all();         //初始化FFMPEG  调用了这个才能正常适用编码器和解码器
}

VideoPlayer::~VideoPlayer()
{
    qCDebug(logPlayer) << "called" << m_strPath;
    this->requestInterruption();
    this->quit();
    this->wait();
    qCDebug(logPlayer) <<  "done" << m_strPath;
}

void VideoPlayer::startPlay(QString strPath)
{
    if (strPath.isEmpty())
        return;

    if (strPath == m_strPath)
        return;

    stopPlay();

    m_strPath = strPath;
    this->start();
}

void VideoPlayer::stopPlay()
{
    if (isRunning())
    {
        this->requestInterruption();
        this->quit();
        this->wait();
    }
}

void VideoPlayer::run()
{
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameRGB;
    AVPacket *packet;
    uint8_t *out_buffer;

    static struct SwsContext *img_convert_ctx;

    int videoStream, i, numBytes;
    int ret, got_picture;

    pFormatCtx = avformat_alloc_context();

    //2017.8.5---lizhen
    AVDictionary *avdic=NULL;
    char option_key[]="rtsp_transport";
    char option_value[]="tcp";
    av_dict_set(&avdic,option_key,option_value,0);
    char option_key2[]="max_delay";
    char option_value2[]="100";
    av_dict_set(&avdic,option_key2,option_value2,0);

    std::string str = m_strPath.toStdString();
    char* url= (char*)str.c_str();

    qCDebug(logPlayer) << "called" << m_strPath;

    if (avformat_open_input(&pFormatCtx, url, NULL, &avdic) != 0) {
        qCCritical(logPlayer) << "can't open the file. \n" << m_strPath;
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        qCCritical(logPlayer) << "Could't find stream infomation.\n" << m_strPath;
        return;
    }

    videoStream = -1;

    ///循环查找视频中包含的流信息，直到找到视频类型的流
    ///便将其记录下来 保存到videoStream变量中
    ///这里我们现在只处理视频流  音频流先不管他
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }
    }

    ///如果videoStream为-1 说明没有找到视频流
    if (videoStream == -1) {
        qCCritical(logPlayer) << "Didn't find a video stream.\n" << m_strPath;
        return;
    }

    ///查找解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    //2017.8.9---lizhen
    pCodecCtx->bit_rate =0;   //初始化为0
    pCodecCtx->time_base.num=1;  //下面两行：一秒钟25帧
    pCodecCtx->time_base.den=10;
    pCodecCtx->frame_number=1;  //每包一个视频帧

    if (pCodec == NULL) {
        qCCritical(logPlayer) << "Codec not found.\n" << m_strPath;
        return;
    }

    ///打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        qCCritical(logPlayer) << "Could not open codec.\n" << m_strPath;
        return;
    }

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    ///这里我们改成了 将解码后的YUV数据转换成RGB32
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
            pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
            AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width,pCodecCtx->height);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_RGB32,
            pCodecCtx->width, pCodecCtx->height);

    int y_size = pCodecCtx->width * pCodecCtx->height;

    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据

    //2017.8.1---lizhen
    av_dump_format(pFormatCtx, 0, url, 0); //输出视频信息

    qCDebug(logPlayer)
             << "start" << m_strPath
             << "pix_fmt" << pCodecCtx->pix_fmt
             << "width" << pCodecCtx->width
             << "height" << pCodecCtx->height;

    while (!this->isInterruptionRequested())
    {
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            qCWarning(logPlayer)  << "read frame error.\n" << m_strPath;
            break; //这里认为视频读取完了
        }

        if (packet->stream_index == videoStream) {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);

            if (ret < 0) {
                qCCritical(logPlayer) << "decode error.\n" << m_strPath;
                return;
            }

            if (got_picture) {
                sws_scale(img_convert_ctx,
                        (uint8_t const * const *) pFrame->data,
                        pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                        pFrameRGB->linesize);

                //把这个RGB数据 用QImage加载
                QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
                emit sig_GetOneFrame(image);  //发送信号
            }
        }

        av_free_packet(packet); //释放资源,否则内存会一直上升

        //2017.8.7---lizhen
        msleep(0.05); //停一停  不然放的太快了
    }

    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    qCDebug(logPlayer) << "done" << m_strPath;
}



