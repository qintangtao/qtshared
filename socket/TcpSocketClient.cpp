#include "tcpsocketclient.h"
#include <QHostAddress>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logTcp, "log.net.tcp.client")

TcpSocketClient::TcpSocketClient(QObject *parent) :
    QTcpSocket (parent),
    m_bHeartbeatEnabled(true),
    m_nHeartbeatTimes(0),
    m_nHeartbeatSend(5),
    m_nHeartbeatTimeout(5),
    m_bHeartbeatDoubleLink(false),
    m_pTcpSocketClient(NULL)
{
    m_timer.setInterval(1000);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onHeartbeatTimeout()));
}

TcpSocketClient::~TcpSocketClient()
{
	disconnectFromServer();

    if (m_pTcpSocketClient)
        m_pTcpSocketClient->disconnectFromServer();
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
    qCDebug(logTcp) << m_ip << m_port << state();

    if(state() == QAbstractSocket::ConnectedState)
        return;

    QTcpSocket::connectToHost(QHostAddress(m_ip), m_port, mode);
    if(msecs > 0 && state() != QAbstractSocket::ConnectedState) {
        waitForConnected(msecs);
    }

    startHeartbeat();
}

void TcpSocketClient::disconnectFromServer(int msecs)
{
    qCDebug(logTcp) << m_ip << m_port << state();

    stopHeartbeat();

    if (m_pTcpSocketClient)
        m_pTcpSocketClient->disconnectFromServer();

    QTcpSocket::disconnectFromHost();
    if(msecs > 0 && state() != QAbstractSocket::UnconnectedState)
        QTcpSocket::waitForDisconnected(msecs);
}

bool TcpSocketClient::waitConnected(int msecs)
{
    qCDebug(logTcp) << m_ip << m_port << state() << msecs;

	QElapsedTimer timer; timer.start();
	while (timer.elapsed() < msecs)
	{
		waitForConnected(50);
        if (state() == QAbstractSocket::ConnectedState) {
			return true;
		}
		QCoreApplication::processEvents();
	}
	return false;
}

qint64 TcpSocketClient::send(const QByteArray& data)
{
    qint64 sendLen = -1;

    if(state() == QAbstractSocket::ConnectedState)
    {
        qCDebug(logTcp) << m_ip << m_port << data << data.toHex();

        sendLen = write(data);
        flush();
    }
    else
    {
        qCWarning(logTcp) << m_ip << m_port << state() << data << data.toHex();
    }

    return sendLen;
}

void TcpSocketClient::onHeartbeatTimeout()
{
    if (m_ip.isEmpty())
        return;

    if (m_nHeartbeatTimes++ < m_nHeartbeatSend)
        return;

    if (m_bHeartbeatDoubleLink) {
        m_pTcpSocketClient->setIPAndPort(m_ip, m_port);
    } else {
        send(m_heartbeatCmd.toLocal8Bit());
    }

    if (m_nHeartbeatTimes > (m_nHeartbeatSend +m_nHeartbeatTimeout ))
    {
        qCWarning(logTcp) << m_ip << m_port << state()
                        << "heartbeatTimes:" << m_nHeartbeatTimes
                        << "heartbeatSend:" << m_nHeartbeatSend
                        << "heartbeatTimeout:" << m_nHeartbeatTimeout
                        << "heartbeatCmd:" << m_heartbeatCmd;

        // 等待设置0，会卡
        if (state() == QAbstractSocket::UnconnectedState) {
            connectToServer(0);
            //m_nHeartbeatTimes = m_nHeartbeatSend - 1;
            m_nHeartbeatTimes = 0; //上面的连接多了会造成卡顿
        } else {
            disconnectFromServer(0);
            // disconnectFromServer 中会停止心跳
            startHeartbeat();
        }
    }
}

void TcpSocketClient::onHeartbeatStateChanged(QAbstractSocket::SocketState state)
{
    qCDebug(logTcp) << state;
    if (state == QAbstractSocket::ConnectedState) {
        m_nHeartbeatTimes = 0;

        // 主链接已断开，此时心跳链接已连接上，需要连接主
        if (this->state() != QAbstractSocket::ConnectedState) {
            disconnectFromServer();
            connectToServer();
        }
    }
}

void TcpSocketClient::startHeartbeat()
{
    if (!m_bHeartbeatEnabled)
        return;

    if (m_bHeartbeatDoubleLink) {
        if (NULL == m_pTcpSocketClient) {
            m_pTcpSocketClient = new TcpSocketClient(this);
            m_pTcpSocketClient->setHeartbeatEnabled(false);
            connect(m_pTcpSocketClient, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onHeartbeatStateChanged(QAbstractSocket::SocketState)));
        }
        if (!m_timer.isActive()) {
            qCDebug(logTcp) << m_ip << m_port << state() << m_bHeartbeatDoubleLink << "start heartbeat";
            m_timer.start();
        }
    } else {
        if (!m_heartbeatCmd.isEmpty()) {
            if (!m_timer.isActive()) {
                qCDebug(logTcp) << m_ip << m_port << state() << m_heartbeatCmd << "start heartbeat";
                m_timer.start();
            }
        } else {
            qCWarning(logTcp) << m_ip << m_port << state() << "heartbeat cmd \"" << m_heartbeatCmd << "\" is empty";
        }
    }
}

void TcpSocketClient::stopHeartbeat()
{
    //m_bHeartbeatEnabled &&   不加此判断，防止中途设置为false
    if (m_timer.isActive()) {
        qCDebug(logTcp) << m_ip << m_port << state() << m_bHeartbeatDoubleLink << m_heartbeatCmd  << "stop heartbeat";
        m_timer.stop();
    }
}
