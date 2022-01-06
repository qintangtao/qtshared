#ifndef TCPSOCKETCLIENT_H
#define TCPSOCKETCLIENT_H
#include <QTcpSocket>
#include <QTimer>

class TcpSocketClient : public QTcpSocket
{
	Q_OBJECT
public:
    TcpSocketClient(QObject *parent = nullptr);
    ~TcpSocketClient();

    void setIPAndPort(const QString &ip, quint16 port, bool connect = true);
    void connectToServer(int msecs = 50, OpenMode mode = ReadWrite);
    void disconnectFromServer(int msecs = 50);
    bool isConnected();
    bool waitConnected(int msecs = 3000);
    qint64 send(const QByteArray &data);

    inline QString ip() const
    { return m_ip; }
    inline quint16 port() const
    { return m_port; }

    /**
     * 是否启用心跳
     * @brief setHeartbeatEnabled
     * @param enabled
     */
    inline void setHeartbeatEnabled(bool enabled)
    { m_bHeartbeatEnabled = enabled; }
    inline bool isHeartbeatEnabled() const
    { return m_bHeartbeatEnabled; }

    /**
     * 每隔几秒发送一次心跳命令
     * @brief setHeartbeatSend
     * @param n
     */
    inline void setHeartbeatSend(int n)
    { m_nHeartbeatSend = n; }
    inline int heartbeatSend() const
    { return m_nHeartbeatSend; }

    /**
     * 从发送命令起，超时几秒重连
     * @brief setHeartbeatTimeout
     * @param n
     */
    inline void setHeartbeatTimeout(int n)
    { m_nHeartbeatTimeout = n; }
    inline int heartbeatTimeout() const
    { return m_nHeartbeatTimeout; }

    /**
     * 心跳命令
     * @brief setHeartbeatCmd
     * @param v
     */
    inline void setHeartbeatCmd(const QString &v)
    { m_heartbeatCmd = v; }
    inline const QString &heartbeatCmd() const
    { return m_heartbeatCmd; }

    /**
     * 内部创建链接检测是否可连
     * @brief setHeartbeatDoubleLink
     * @param v
     */
    inline void setHeartbeatDoubleLink(bool b)
    { m_bHeartbeatDoubleLink = b; }
    inline const bool &heartbeatDoubleLink() const
    { return m_bHeartbeatDoubleLink; }

    /**
     * 收到心跳回送命令后，清除心跳计数器
     * @brief clearHeartbeatTimes
     */
    inline void clearHeartbeatTimes()
    { m_nHeartbeatTimes = 0; }

Q_SIGNALS:

private Q_SLOTS:
    void onHeartbeatTimeout();
    void onHeartbeatStateChanged(QAbstractSocket::SocketState);

private:
    QString m_ip;
    quint16 m_port;

    //heartbeat
    QTimer m_timer;
    //是否启用心跳
    bool m_bHeartbeatEnabled;
    //心跳计数器
    int m_nHeartbeatTimes;
    //每隔几秒发送一次心跳命令
    int m_nHeartbeatSend;
    //从发送命令起，超时几秒重连
    int m_nHeartbeatTimeout;
    //心跳命令
    QString m_heartbeatCmd;
    //内部创建链接检测是否可连
    bool m_bHeartbeatDoubleLink;

    // 内部心跳检测
    TcpSocketClient *m_pTcpSocketClient;

private:
    Q_DISABLE_COPY(TcpSocketClient)
};


#endif // TCPSOCKETCLIENT_H
