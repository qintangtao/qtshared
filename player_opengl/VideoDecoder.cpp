#include "VideoDecoder.h"
#include <QDebug>
#include <QDateTime>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logPlayer, "log.player.decoder")

extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libavcodec/avfft.h"
	#include "libavdevice/avdevice.h"
	#include "libswscale/swscale.h"
	#include "libavformat/avformat.h"
	#include "libavformat/avio.h"
	#include "libavutil/common.h"
	#include "libavutil/avstring.h"
	#include "libavutil/imgutils.h"
	#include "libavutil/time.h"
	#include "libavutil/frame.h"
	#include "libswresample/swresample.h"
}

static int interrupt_callback(void *p)
{
	VideoDecoder *r = (VideoDecoder *)p;
	if (r->isQuit())
		return 1;

	if (r->lasttime() > 0 &&
        (time(NULL) - r->lasttime() > 30)) {
		return 1;
	}

	return 0;
}

VideoDecoder::VideoDecoder(QObject *parent)
	: QThread(parent)
	, m_pFrame(NULL)
	, m_pFrameYUV(NULL)
	, m_pFormatCtx(NULL)
	, m_pCodecCtx(NULL)
	, m_pFramePacket(NULL)
	, m_pSwsContext(NULL)
	, m_pOutBuffer(NULL)
	, m_bQuit(false)
	, m_bPause(false)
	, m_bIsPlaying(false)
	, m_lasttime(0)
{
	av_register_all();
	avformat_network_init();
}


VideoDecoder::~VideoDecoder()
{
	this->stop();
}

void VideoDecoder::start(const QString &url)
{
	if (this->isRunning())
		return;

	if (url.isEmpty())
		return;

	m_strUrl = url;

	m_bQuit = false;
	m_bPause = false;
	QThread::start();
}

void VideoDecoder::start()
{
	if (this->isRunning())
		return;

	if (m_strUrl.isEmpty())
		return;

	m_bQuit = false;
	m_bPause = false;
	QThread::start();
}

void VideoDecoder::stop()
{
	if (!this->isRunning())
		return;

	m_bQuit = true;
	m_bPause = false;
	this->requestInterruption();
	this->quit();
	this->wait();
}

void VideoDecoder::togglePause()
{
	if (!this->isRunning())
		return;

	m_bPause = !m_bPause;
}

void VideoDecoder::run()
{
    int video_display_count = 0;

	while (!m_bQuit && !this->isInterruptionRequested())
	{
		if (initData() > 0)
		{
			while (!m_bQuit)
            {
                this->m_lasttime = time(NULL);

				if (av_read_frame(m_pFormatCtx, m_pFramePacket) < 0)
					break;

                if (m_bPause) {
                    video_display_count = 0;
                    av_packet_unref(m_pFramePacket);
                    QThread::msleep(50);
                    continue;
                }

				if (m_pFormatCtx->streams[m_pFramePacket->stream_index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
				{
					int recive = avcodec_send_packet(m_pCodecCtx, m_pFramePacket);
					while (!m_bQuit && recive == 0)
					{
						recive = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
						if (recive != 0)
							break; 

						sws_scale(m_pSwsContext, (const uint8_t *const*)m_pFrame->data, m_pFrame->linesize, 0, 
							m_pCodecCtx->height, m_pFrameYUV->data, m_pFrameYUV->linesize);

                        //qCDebug(logPlayer) << "showYUV" << m_strUrl << m_nBuffSize << m_pCodecCtx->width << m_pCodecCtx->height;
                        if (video_display_count++ > 20)
                        {
                            video_display_count = 20;
                            emit showYUV(QByteArray((const char *)m_pOutBuffer, m_nBuffSize * sizeof(uint8_t)), m_pCodecCtx->width, m_pCodecCtx->height);
                        }
					}
				}
				av_packet_unref(m_pFramePacket);
			}
		}
		freeData();
	}
	freeData();
}

int VideoDecoder::initData()
{
	m_pFormatCtx = avformat_alloc_context();

    qCDebug(logPlayer) << m_strUrl;

	AVDictionary *pOptions = NULL;
	if (!av_dict_get(pOptions, "rtsp_transport", NULL, 0)) {
        av_dict_set(&pOptions, "rtsp_transport", "tcp", 0);
        av_dict_set(&pOptions, "stimeout", "30000000", 0); //设置超时断开连接时间
		av_dict_set(&pOptions, "buffer_size", "2097152", 0); //设置缓存大小，1080p可将值调大
		AVDictionaryEntry *dict = av_dict_get(pOptions, "buffer_size", NULL, 0);
        qCDebug(logPlayer) << "dict" << dict->key << dict->value;
	}

#if 0
	if (avformat_open_input(&m_pFormatCtx, m_strUrl.toLocal8Bit(), NULL, &pOptions) != 0) {
		qDebug() << "open filed==" << m_strUrl;
		return 0;
#else
	m_pFormatCtx->interrupt_callback.callback = interrupt_callback;
	m_pFormatCtx->interrupt_callback.opaque = this;
	this->m_lasttime = time(NULL);

	if (avformat_open_input(&m_pFormatCtx, m_strUrl.toLocal8Bit(), NULL, &pOptions) != 0) {
        qCCritical(logPlayer) << "open filed==" << m_strUrl;
		return 0;
	}

	if (m_bQuit)
		return 0;

#if 0
	pOptions = NULL;
	if (!av_dict_get(pOptions, "rtsp_transport", NULL, 0)) {
		av_dict_set(&pOptions, "rtsp_transport", "tcp", 0);
		av_dict_set(&pOptions, "stimeout", "5000000", 0); //设置超时断开连接时间
		av_dict_set(&pOptions, "buffer_size", "2097152", 0); //设置缓存大小，1080p可将值调大
		AVDictionaryEntry *dict = av_dict_get(pOptions, "buffer_size", NULL, 0);
        qCDebug(logPlayer) << "dict" << dict->key << dict->value;
	}

	//设置回调会造成读取数据错误,释放不设置回调重新打开流测试
	avformat_close_input(&m_pFormatCtx);
	if (m_pFormatCtx) {
		avformat_free_context(m_pFormatCtx);
	}
	m_pFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&m_pFormatCtx, m_strUrl.toLocal8Bit(), NULL, &pOptions) != 0) {
        qCCritical(logPlayer) << "open filed==" << m_strUrl;
		return 0;
	}
#endif
#endif

	if (m_bQuit)
		return 0;

	if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0) { //读取文件部分内容 如时长等信息
        qCCritical(logPlayer) << "avformat_find_stream_info failed" << m_strUrl;
		return 0;
	}

	m_iVideoIndex = -1;
	for (unsigned int i = 0; i < m_pFormatCtx->nb_streams; i++) {
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			m_iVideoIndex = i;
			break;
		}

	}
	if (m_iVideoIndex == -1) {
        qCCritical(logPlayer)  << "not found AVMEDIA_TYPE_VIDEO" << m_strUrl;
		return 0;
	}

	//根据视频索引定位AVCodec
	m_pCodecCtx = m_pFormatCtx->streams[m_iVideoIndex]->codec;
	AVCodec *pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
	if (pCodec == NULL) {
        qCCritical(logPlayer) << "avcodec_find_decoder" << m_pCodecCtx->codec_id << m_strUrl;
		return 0;
	}

	if (avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0) {
        qCCritical(logPlayer) << "avcodec_open2" << m_strUrl;
		return 0;
	}

	if (m_pCodecCtx->width == 0 || m_pCodecCtx->height == 0) {

        qCCritical(logPlayer) << "width height is 0" << m_strUrl;
		return 0;
	}

	av_dump_format(m_pFormatCtx, 0, m_strUrl.toLocal8Bit().data(), 0);

	m_pFrame = av_frame_alloc();
	m_pFrameYUV = av_frame_alloc();
	m_pFramePacket = av_packet_alloc();

	m_pSwsContext = sws_getContext(m_pCodecCtx->width, m_pCodecCtx->height, m_pCodecCtx->pix_fmt, 
		m_pCodecCtx->width, m_pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);

	m_nBuffSize = avpicture_get_size(AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height);
	m_pOutBuffer = (uint8_t*)av_malloc(m_nBuffSize * sizeof(uint8_t));
	avpicture_fill((AVPicture*)m_pFrameYUV, m_pOutBuffer, AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height);

    qCDebug(logPlayer) << "open success" << m_strUrl;

	return 1;
}

void VideoDecoder::freeData()
{
	if (m_pFrame) {
		av_frame_free(&m_pFrame);
		m_pFrame = NULL;
	}

	if (m_pFrameYUV) {
		av_frame_free(&m_pFrameYUV);
		m_pFrameYUV = NULL;
	}

	if (m_pCodecCtx) {
		avcodec_close(m_pCodecCtx);
		m_pCodecCtx = NULL;
	}

	if (m_pFormatCtx) {
		avformat_close_input(&m_pFormatCtx);
		if (m_pFormatCtx != NULL)
			avformat_free_context(m_pFormatCtx);
		m_pFormatCtx = NULL;
	}

	if (m_pFramePacket) {
		av_packet_free(&m_pFramePacket);
		m_pFramePacket = NULL;
	}

	if (m_pSwsContext) {
		sws_freeContext(m_pSwsContext);
		m_pSwsContext = NULL;
	}

	if (m_pOutBuffer) {
		av_free(m_pOutBuffer);
		m_pOutBuffer = NULL;
	}
}
