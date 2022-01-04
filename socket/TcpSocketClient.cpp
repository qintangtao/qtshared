#include "tcpsocketclient.h"
#include <QHostAddress>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QDebug>

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include <QLoggingCategory>
Q_LOGGING_CATEGORY(logTcp, "log.net.tcp.client")
#endif

TcpSocketClient::TcpSocketClient(QObject *parent) :
    QTcpSocket (parent),
    m_bHeartbeatEnabled(true),
    m_nHeartbeatTimes(0),
    m_nHeartbeatSend(5),
    m_nHeartbeatTimeout(5)
{
    m_timer.setInterval(1000);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onHeartbeatTimeout()));
}

TcpSocketClient::~TcpSocketClient()
{
	disconnectFromServer();
}

void TcpSocketClient::setIPAndPort(const QString &ip, quint16 port, bool connect)
{
	m_ip = ip;
    m_port = port;
    if (connect) {
        disconnectFromServer();
        connectToServer();
    }
}

void TcpSocketClient::connectToServer(int msecs, OpenMode mode)
{
    if(isConnected())
        return;

    QTcpSocket::connectToHost(QHostAddress(m_ip), m_port, mode);
    if(msecs > 0) {
        waitForConnected(msecs);
    }

    if (m_bHeartbeatEnabled) {
        if (!m_heartbeatCmd.isEmpty()) {
            if (!m_timer.isActive()) {
                qCDebug(logTcp) << m_ip << m_port << isConnected() << m_heartbeatCmd << "start heartbeat";
                m_timer.start();
            }
        } else {
            qCWarning(logTcp) << m_ip << m_port << isConnected() << "heartbeat cmd \"" << m_heartbeatCmd << "\" is empty";
        }
    }
}

void TcpSocketClient::disconnectFromServer(int msecs)
{
    if(!isConnected())
        return;

    //m_bHeartbeatEnabled &&   不加此判断，防止中途设置为false
    if (m_timer.isActive()) {
        qCDebug(logTcp) << m_ip << m_port << isConnected() << m_heartbeatCmd << "stop heartbeat";
        m_timer.stop();
    }

    QTcpSocket::disconnectFromHost();
    if(msecs > 0)
        QTcpSocket::waitForDisconnected(msecs);
}

bool TcpSocketClient::isConnected()
{
    return state() == QAbstractSocket::ConnectedState;
}

bool TcpSocketClient::waitConnected(int msecs)
{
	QElapsedTimer timer; timer.start();
	while (timer.elapsed() < msecs)
	{
		waitForConnected(50);
		if (isConnected()) {
			return true;
		}
		QCoreApplication::processEvents();
	}
	return false;
}

qint64 TcpSocketClient::send(const QByteArray& data)
{
    qint64 sendLen = -1;

    qCDebug(logTcp) << m_ip << m_port << data << data.toHex();

    if(isConnected())
    {
        sendLen = write(data);
        flush();
    }
    else
    {
        qCWarning(logTcp) << m_ip << m_port << "disconnect";
    }

    return sendLen;
}

void TcpSocketClient::onHeartbeatTimeout()
{
    if (m_nHeartbeatTimes++ < m_nHeartbeatSend)
        return;

    send(m_heartbeatCmd.toLocal8Bit());

    if (m_nHeartbeatTimes > (m_nHeartbeatSend +m_nHeartbeatTimeout ))
    {
        qCWarning(logTcp) << m_ip << m_port << isConnected()
                        << m_nHeartbeatTimes
                        << m_nHeartbeatSend
                        << m_nHeartbeatTimeout
                        << m_heartbeatCmd;

        m_nHeartbeatTimes = m_nHeartbeatSend - 1;
        disconnectFromServer();
        connectToServer();
    }
}
