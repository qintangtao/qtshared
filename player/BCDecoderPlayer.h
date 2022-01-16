/*********************************************************************************************************************************
* 作    者：liuwl
* 摘    要：数据源窗口内的解码器输入，实则为IPVedio的输入链表，提供选择文件、筛选、拖拽信号的功能
*********************************************************************************************************************************/
#ifndef BCDecoderPlayer_H
#define BCDecoderPlayer_H

#include <QLabel>
#include <QThread>
#include <QTimer>

class VideoPlayer: public QThread
{
    Q_OBJECT
public:
    VideoPlayer(QObject *parent = Q_NULLPTR);
    ~VideoPlayer();

    void startPlay(QString strPath);
    void stopPlay();

protected:
    void run();

signals:
    void sig_GetOneFrame(QImage);

private:
    QString m_strPath;
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

public slots:
    void SetImageSlots(QImage);
    void play(QString url);
    void stop();
    void onTimeout();

private:
    VideoPlayer *m_VideoPlayer;

    QTimer m_timer;
    QPixmap m_defaultPixmap;
    QString m_loadTip;
    QString m_loadTimeoutTip;
};



#endif // BCDECODER_H
