#ifndef VideoDecoder_H
#define VideoDecoder_H
#include <QThread>

class AVFrame;
class AVFormatContext;
class AVCodecContext;
class AVPacket;
class SwsContext;

class VideoDecoder :
	public QThread
{
	Q_OBJECT
public:
	VideoDecoder(QObject *parent = nullptr);
	~VideoDecoder();


	void start(const QString &url);
	void start();
	void stop();

	void togglePause();

	bool isQuit() const;
	QString url() const;

	time_t lasttime() const;

Q_SIGNALS:
	void showYUV(QByteArray byte, int width, int height);

protected:
	void run();

	int initData();
	void freeData();

private:
	AVFrame *m_pFrame;
	AVFrame* m_pFrameYUV;
	AVFormatContext *m_pFormatCtx;
	AVCodecContext *m_pCodecCtx;
	AVPacket *m_pFramePacket;
	SwsContext* m_pSwsContext;

	int  m_nBuffSize;
	uint8_t *m_pOutBuffer;

	int m_iVideoIndex;

	bool m_bQuit; 
	bool m_bPause;
	bool m_bIsPlaying;

	time_t m_lasttime;

	QString m_strUrl;
};

inline bool VideoDecoder::isQuit() const {
	return m_bQuit;
}

inline QString VideoDecoder::url() const {
	return m_strUrl;
}

inline time_t VideoDecoder::lasttime() const {
	return m_lasttime;
}

#endif // VideoDecoder_H
