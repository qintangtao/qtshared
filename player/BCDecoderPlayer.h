/*********************************************************************************************************************************
* 作    者：liuwl
* 摘    要：数据源窗口内的解码器输入，实则为IPVedio的输入链表，提供选择文件、筛选、拖拽信号的功能
*********************************************************************************************************************************/
#ifndef BCDecoderPlayer_H
#define BCDecoderPlayer_H

#include <QLabel>
#include <QThread>

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

public slots:
    void SetImageSlots(QImage);
    void play(QString url);
    void stop();

private:
    VideoPlayer *m_VideoPlayer;
};



#endif // BCDECODER_H
