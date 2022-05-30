/*********************************************************************************************************************************
* 作    者：liuwl
* 摘    要：数据源窗口内的解码器输入，实则为IPVedio的输入链表，提供选择文件、筛选、拖拽信号的功能
*********************************************************************************************************************************/
#ifndef BCDecoderPlayer_H
#define BCDecoderPlayer_H

#include <QLabel>
#include <QThread>
#include <QTimer>
#include <QMap>

class VideoPlayer: public QThread
{
    Q_OBJECT
public:
    VideoPlayer(int id, const QString &url, QObject *parent = Q_NULLPTR);
    ~VideoPlayer();

    void startDec();

    void stopDec();

    inline time_t lasttime() const
    { return m_lasttime; }

    inline time_t timeout() const
    { return m_timeout; }

    inline int id() const
    { return m_id; }

    inline const QString &path() const
    { return m_strPath; }

    inline int refCount() const
    { return m_nRefCount; }

private slots:
    void onDelayStop();

protected:

    void startPlay();
    void stopPlay();

    void run();


signals:
    void sig_GetOneFrame(QImage);

private:
    int m_id;
    int m_nRefCount;
    QString m_strPath;
    time_t m_lasttime;
    time_t m_timeout;

    QTimer m_timer;
};

class BCDecoderPlayer : public QLabel
{
    Q_OBJECT

public:
    explicit BCDecoderPlayer(QWidget *parent = 0);
    ~BCDecoderPlayer();

    inline void setTimeout(int msec)
    { m_timer.setInterval(msec); }
    inline void setDefaultPixmap(const QPixmap &pix)
    { m_defaultPixmap = pix; }
    inline void setLoadTip(const QString &text)
    { m_loadTip = text; }
    inline void setLoadTimeoutTip(const QString &text)
    { m_loadTimeoutTip = text; }

    bool addVideoPlayer(int id, const QString &url);

public slots:
    void SetImageSlots(QImage);
    void play(int id);
    void stop();
    void onTimeout();

private:
    //VideoPlayer *m_VideoPlayer;

    QMap<int, VideoPlayer *> m_mapVideoPlayer;
    int m_nCurrentId;

    QTimer m_timer;
    QPixmap m_defaultPixmap;
    QString m_loadTip;
    QString m_loadTimeoutTip;
};



#endif // BCDECODER_H
