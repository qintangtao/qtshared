#include "BCDecoderPlayer.h"
#include <QMutex>
#include <QHoverEvent>
#include <QDebug>
#include <QTime>
#include <QResizeEvent>
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

    #include "SDL2/SDL.h"
    #include "SDL2/SDL_events.h"
    #include "SDL2/SDL_thread.h"
}
#include <windows.h>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logPlayer, "log.player.decoder")


class BCVedioWindow
{
public:
    BCVedioWindow(HWND wid, const QRect &rect);
    ~BCVedioWindow();

    void Update(const QRect &rect);

    HWND m_wid;     // 窗口ID
    QRect m_rect;   // 绘制尺寸

    SDL_Window *pScreen;
    SDL_Renderer *pRenderer;
    SDL_Texture *pYUVTexture;
};

BCVedioWindow::BCVedioWindow(HWND wid, const QRect &rect)
{
    m_wid = wid;
    m_rect = rect;
    pScreen = SDL_CreateWindowFrom( m_wid );
    pRenderer = SDL_CreateRenderer(pScreen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    pYUVTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET, m_rect.width(), m_rect.height());
}

BCVedioWindow::~BCVedioWindow()
{
    SDL_Delay(50);
    SDL_DestroyTexture(pYUVTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pScreen);

    pYUVTexture = NULL;
    pRenderer = NULL;
    pScreen = NULL;

    // 必须添加，否则再次开启时不显示
    ShowWindow(m_wid, SW_SHOWNA);
    UpdateWindow(m_wid);
}

void BCVedioWindow::Update(const QRect &rect)
{
    if (m_rect == rect)
        return;

    m_rect = rect;

    SDL_DestroyTexture(pYUVTexture);
    SDL_DestroyRenderer(pRenderer);
    pRenderer = SDL_CreateRenderer(pScreen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    pYUVTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING | SDL_TEXTUREACCESS_TARGET, m_rect.width(), m_rect.height());
}



// ---------------------------------------------------------------------------------------------------------------------------

BCDecoderPlayer::BCDecoderPlayer(QWidget *parent) :
    QLabel(parent)
{
    m_pVideoPlayer = new VideoPlayer(0, "", this);
}

BCDecoderPlayer::BCDecoderPlayer(const QString &url, QWidget *parent) :
    QLabel(parent)
{
    this->setAlignment(Qt::AlignCenter);

    m_defaultPixmap.fill(QColor(0,0,0));
    m_loadTip = tr("加载中...");
    m_loadTimeoutTip = tr("加载出错, 请重试! ");

    m_pVideoPlayer = new VideoPlayer(0, url, this);
    connect(m_pVideoPlayer,SIGNAL(sig_GetOneFrame(QImage)),this,SLOT(SetImageSlots(QImage)), Qt::QueuedConnection);

    m_timer.setInterval(30000);
    m_timer.setSingleShot(true);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(onTimeout()));
}

BCDecoderPlayer::~BCDecoderPlayer()
{
    m_timer.stop();
    m_pVideoPlayer->deleteLater();
}


void BCDecoderPlayer::resizeEvent(QResizeEvent *e)
{
    QLabel::resizeEvent(e);
    m_pVideoPlayer->UpdateEcho((HWND)winId(), this->rect());
}

void BCDecoderPlayer::SetImageSlots(const QImage image)
{
    m_timer.stop();
    if (image.height() > 0){
        QPixmap pix = QPixmap::fromImage(image);
        this->setPixmap(pix.scaled(this->size()));
    }
}

void BCDecoderPlayer::play()
{
#if 0
    stop();

    if (m_pVideoPlayer->refCount() == 0)
    {
        this->setPixmap(m_defaultPixmap);
        this->setText(m_loadTip);
        m_timer.start();
    }
#endif

    m_pVideoPlayer->AddEcho((HWND)winId(), this->rect());
    setUpdatesEnabled( false );

    if (!m_pVideoPlayer->isRunning())
    {
        //this->setPixmap(m_defaultPixmap);
        //this->setText(m_loadTip);
        //m_timer.start();
    } else {
        return;
    }

    m_pVideoPlayer->startDec();
}

void BCDecoderPlayer::stop()
{
    m_pVideoPlayer->RemoveEcho((HWND)winId());
    setUpdatesEnabled( true );

    //m_pVideoPlayer->stopDec();
}

void BCDecoderPlayer::onTimeout()
{
    this->setText(m_loadTimeoutTip);
}

/**
 * @brief VideoPlayer::VideoPlayer
 * @param parent
 */
static int nqsrand = 0;
static int max = 18000; //180;
static int min = 6000; //60;

VideoPlayer::VideoPlayer(int id, const QString &url, QObject *parent)
    : QThread(parent)
    , m_timeout(30)
    , m_id(id)
    , m_strPath(url)
    , m_nRefCount(0)
{
    static bool init_ffmpeg = false;
    if (!init_ffmpeg) {
        init_ffmpeg = true;
        avformat_network_init();   //初始化FFmpeg网络模块，2017.8.5---lizhen
        av_register_all();         //初始化FFMPEG  调用了这个才能正常适用编码器和解码器
        // init SDL
        SDL_SetMainReady();
        qCDebug(logPlayer) << "SDL_Init: " << SDL_Init( SDL_INIT_VIDEO|SDL_INIT_TIMER );
        SDL_EventState(SDL_WINDOWEVENT, SDL_IGNORE);
        SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
        SDL_EventState(SDL_USEREVENT, SDL_IGNORE);
    }

    qsrand(time(NULL) + nqsrand);
    nqsrand += 40;
    int num = qrand() % (max-min) + min;

    qCDebug(logPlayer) << "max" << max << "min" <<min << "num" <<num;

    m_timer.setInterval(num * 1000);
    m_timer.setSingleShot(true);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(onDelayStop()));
}

VideoPlayer::~VideoPlayer()
{
    stopPlay();
}


void VideoPlayer::startDec()
{
    m_nRefCount++;

    //if (m_timer.isActive())
    //    m_timer.stop();

    qCDebug(logPlayer) << "v9" << m_id << m_strPath << m_nRefCount << isRunning();

    if (this->isRunning())
        return;

    startPlay();
}

void VideoPlayer::stopDec()
{
    if (m_nRefCount == 0)
        return;

    m_nRefCount--;

    //if (m_nRefCount == 0 && !m_timer.isActive()) {
    //    m_timer.start();
    //}

    qCDebug(logPlayer) << m_id << m_strPath << m_nRefCount << isRunning();
}

// 添加回显
void VideoPlayer::AddEcho(HWND wid, const QRect &rect)
{
    QMutexLocker locker(&m_mutexWindow);
    QListIterator<BCVedioWindow *> it( m_lstWindow );
    while ( it.hasNext() ) {
        BCVedioWindow *pWindow = it.next();

        // 如果已经添加过了直接返回
        if (pWindow->m_wid == wid) {
            return;
        }
    }

    // 新建窗口类添加到链表
     m_lstWindow.append( new BCVedioWindow(wid, rect) );
}

// 更新回显单元
void VideoPlayer::UpdateEcho(HWND wid, const QRect &rect)
{
    // 查找窗口并跳出循环
    QMutexLocker locker(&m_mutexWindow);
    //m_mutexWindow.lock();
    QListIterator<BCVedioWindow *> it( m_lstWindow );
    while ( it.hasNext() ) {
        BCVedioWindow *pWindow = it.next();

        // 如果已经添加过了直接返回
        if (pWindow->m_wid != wid)
            continue;

        pWindow->Update(rect);

        break;
    }
}

// 删除回显
void VideoPlayer::RemoveEcho(HWND wid)
{
    // 查找窗口并移除链表
    QMutexLocker locker(&m_mutexWindow);
    QListIterator<BCVedioWindow *> it( m_lstWindow );
    while ( it.hasNext() ) {
        BCVedioWindow *pWindow = it.next();

        // 如果已经添加过了直接返回
        if (pWindow->m_wid != wid)
            continue;

        m_lstWindow.removeOne( pWindow );

        // 销毁对象
        delete pWindow;
        pWindow = NULL;

        break;
    }
}

void VideoPlayer::onDelayStop()
{
    if (m_nRefCount == 0)
        stopPlay();
}

void VideoPlayer::startPlay()
{
    qCDebug(logPlayer) << m_id << m_strPath << isRunning();

    if (isRunning())
        return;

    this->start();
}

void VideoPlayer::stopPlay()
{
    qCDebug(logPlayer) << m_id << m_strPath << isRunning();

    if (!isRunning())
        return;

    this->requestInterruption();
    //this->quit();
    this->wait(300);
}

// 回调函数
static int interrupt_callback(void *p)
{
    VideoPlayer *r = (VideoPlayer *)p;

    if (r->isInterruptionRequested()) {
        qCWarning(logPlayer) << "quit" << r->path();
        return 1;
    }

    if (r->lasttime() > 0 &&
        ((time(NULL) - r->lasttime() )> r->timeout())) {
        qCWarning(logPlayer) << "timeout" << r->path();
        return 1;
    }

    return 0;
}

void VideoPlayer::run()
{
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameRGB, *pFrameYUV;
    AVPacket pkt;
    uint8_t *out_buffer;

    uint8_t		*DesData[4];
    int			DesLinesize[4];

    static struct SwsContext *img_convert_ctx;

    int videoStream, i, numBytes;
    int ret, got_picture;


    pFormatCtx = avformat_alloc_context();

    //2017.8.5---lizhen
    AVDictionary *avdic=NULL;
    av_dict_set(&avdic,"rtsp_transport","tcp",0);
    //av_dict_set(&avdic,"max_delay","100",0);
    av_dict_set(&avdic, "stimeout", "30000000", 0);
    //av_dict_set(&avdic, "timeout", "30000000", 0);

    std::string str = m_strPath.toStdString();
    char* url= (char*)str.c_str();

    qCDebug(logPlayer) << "called" << m_strPath;// << pCodec->id;

#if 1
   pFormatCtx->interrupt_callback.callback = interrupt_callback;
   pFormatCtx->interrupt_callback.opaque = this;
   this->m_lasttime = time(NULL);
#endif

    if (avformat_open_input(&pFormatCtx, url, NULL, &avdic) != 0) {
        qCCritical(logPlayer) << "can't open the file." << m_strPath;
        avformat_free_context(pFormatCtx);
        return;
    }

    qCDebug(logPlayer) << "open success" << m_strPath;

    this->m_lasttime = time(NULL);
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        qCCritical(logPlayer) << "Could't find stream infomation." << m_strPath;
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
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
        qCCritical(logPlayer) << "Didn't find a video stream." << m_strPath;
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        return;
    }

    ///查找解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    //2017.8.9---lizhen
#if 0
    pCodecCtx->bit_rate =0;   //初始化为0
    pCodecCtx->time_base.num=1;  //下面两行：一秒钟25帧
    pCodecCtx->time_base.den=10;
    pCodecCtx->frame_number=1;  //每包一个视频帧
#endif

    if (pCodec == NULL) {
        qCCritical(logPlayer) << "Codec not found." << m_strPath;
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        return;
    }

    ///打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        qCCritical(logPlayer) << "Could not open codec." << m_strPath;
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
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

    //av_image_alloc(DesData, DesLinesize, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, 1);
    av_image_alloc(DesData, DesLinesize, 3840, 2160, AV_PIX_FMT_YUV420P, 1);

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    //2017.8.1---lizhen
    av_dump_format(pFormatCtx, 0, url, 0); //输出视频信息

    qCDebug(logPlayer)
             << "start" << m_strPath
             << "pix_fmt" << pCodecCtx->pix_fmt
             << "width" << pCodecCtx->width
             << "height" << pCodecCtx->height
             << "refCount" << m_nRefCount;

   // bool flush_buffers = false;

    while (!this->isInterruptionRequested())
    {
#if 0
        if ( m_nRefCount < 1 )
        {
            flush_buffers = true;
            msleep(0.05);
            continue;
        }

        if (flush_buffers) {
            flush_buffers = false;
            avcodec_flush_buffers(pCodecCtx);
        }
#endif

        this->m_lasttime = time(NULL);

        if (av_read_frame(pFormatCtx, &pkt) < 0)
        {
            qCWarning(logPlayer)  << "read frame error." << m_strPath;
            break; //这里认为视频读取完了
        }

        if (this->isInterruptionRequested())
            break;

        if (pkt.stream_index == videoStream)
        {
            this->m_lasttime = time(NULL);

            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &pkt);

            if (ret < 0)
            {
                qCCritical(logPlayer) << "decode error." << m_strPath;
                break;
            }

            if (this->isInterruptionRequested())
            {
                av_frame_unref(pFrame);
                break;
            }

            if (got_picture)
            {
#if 1
                QListIterator<BCVedioWindow *> it( m_lstWindow );
                while ( it.hasNext() ) {
                    if ( this->isInterruptionRequested() )
                        break;

                    QMutexLocker locker(&m_mutexWindow);

                    BCVedioWindow *pWindow = it.next();
                    if (-1 == m_lstWindow.indexOf( pWindow ))
                        continue;

                    int srcWidth = pCodecCtx->width;
                    int srcHeight = pCodecCtx->height;
                    int destWidth = pWindow->m_rect.width();
                    int destHeight = pWindow->m_rect.height();

                    struct SwsContext *img_convert_ctx2 = sws_getContext(srcWidth, srcHeight, pCodecCtx->pix_fmt, destWidth, destHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
                    if (NULL != img_convert_ctx2) {
                        sws_scale(img_convert_ctx2,
                                  (const uint8_t* const*)pFrame->data,
                                  pFrame->linesize, 0, srcHeight,
                                  DesData, DesLinesize);
                        sws_freeContext(img_convert_ctx2);
                    }

                    if ( this->isInterruptionRequested() )
                        break;

                    if (-1 != m_lstWindow.indexOf(pWindow)) {
                        // 这里直接用SDL渲染带句柄的窗口
                        SDL_Rect WinRect;
                        WinRect.x = 0;
                        WinRect.y = 0;
                        WinRect.w = destWidth;
                        WinRect.h = destHeight;

                        SDL_RenderClear(pWindow->pRenderer);

                        // YUV render
                        SDL_UpdateYUVTexture(pWindow->pYUVTexture, &WinRect,
                                        DesData[0], DesLinesize[0],
                                        DesData[1], DesLinesize[1],
                                        DesData[2], DesLinesize[2]);
                        SDL_RenderCopy(pWindow->pRenderer, pWindow->pYUVTexture, &WinRect, NULL);

                        SDL_RenderPresent(pWindow->pRenderer);
                    }
                }

#else
                sws_scale(img_convert_ctx,
                        (uint8_t const * const *) pFrame->data,
                        pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                        pFrameRGB->linesize);

                if (this->isInterruptionRequested())
                {
                    av_frame_unref(pFrame);
                    break;
                }

                //if (m_nRefCount > 0)
                {
                    //把这个RGB数据 用QImage加载
                    QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                    QImage image = tmpImg.copy(); //把图像复制一份 传递给界面显示
                    emit sig_GetOneFrame(image);  //发送信号
                }
#endif
            }

            av_frame_unref(pFrame);
        }

        av_packet_unref(&pkt);

        //2017.8.7---lizhen
        msleep(0.05); //停一停  不然放的太快了
    }

    av_packet_unref(&pkt);
    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    avformat_free_context(pFormatCtx);

    memset(DesData, 0, sizeof(DesData));
    memset(DesLinesize, 0, sizeof(DesLinesize));
    av_freep(&DesData[0]);

    qCDebug(logPlayer) << "done" << m_strPath;
}
