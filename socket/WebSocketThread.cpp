#include "WebSocketThread.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logWeb, "log.net.web.client")

WebSocketThread::WebSocketThread(QObject *parent) : QThread(parent)
{

}
WebSocketThread::~WebSocketThread()
{
    stop();
}

void WebSocketThread::stop()
{
    this->requestInterruption();
    this->quit();
    this->wait();
}

void WebSocketThread::send(const QString& url)
{
    QMutexLocker locker(&m_mutexUrl);
    m_listUrl.append(url);
}

void WebSocketThread::run()
{
    while (!this->isInterruptionRequested())
    {
        if (m_listUrl.isEmpty()) {
            QThread::msleep(100);
            continue;
        }

        m_mutexUrl.lock();
        QString url = m_listUrl.takeFirst();
        m_mutexUrl.unlock();

        open(url);

        //QThread::msleep(100);
    }
    
}

void WebSocketThread::open(const QString &url)
{
    qCDebug(logWeb) << url;

    QTimer timer;
    timer.setInterval(3000);  // 设置超时时间 30 秒
    timer.setSingleShot(true);  // 单次触发

    QNetworkAccessManager manager;
    QNetworkReply *pReply = manager.get(QNetworkRequest(url));

    QEventLoop loop;
    connect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec();

    if (timer.isActive()) {  // 处理响应
        timer.stop();
        if (pReply->error() != QNetworkReply::NoError) {
            // 错误处理
            qCWarning(logWeb) << "Error String : " << pReply->errorString();
        }
        else {
            QVariant variant = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            int nStatusCode = variant.toInt();
            // 根据状态码做进一步数据处理
            //QByteArray bytes = pReply->readAll();
            qCDebug(logWeb) << "Status Code : " << nStatusCode;
        }
    }
    else {  // 处理超时
        disconnect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
        pReply->abort();
        pReply->deleteLater();
        qCDebug(logWeb) << "Timeout";
    }
}
