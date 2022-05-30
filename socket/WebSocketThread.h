#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H
#include <QThread>
#include <QMutex>


class WebSocketThread : public QThread {
    Q_OBJECT
public:
    explicit WebSocketThread(QObject *parent = nullptr);
    ~WebSocketThread();

    void stop();
    void send(const QString& url);

protected:
    void run();
    void open(const QString &url);

private:
    QList<QString> m_listUrl;
    QMutex m_mutexUrl;

private:
    Q_DISABLE_COPY(WebSocketThread)
};

#endif // WEBSOCKETCLIENT_H
