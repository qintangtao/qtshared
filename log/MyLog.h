#ifndef MYLOG_H
#define MYLOG_H

#include <QObject>
#include <QMap>
class Appender;
class QFile;
class MyLog : public QObject
{
    Q_OBJECT
public:

    enum AppenderName {
        FileAppender = 0,
        ConsoleAppender = 1,
        UserAppender = 2 //用户自定义Appender
    };

    static MyLog *Logger();

    static void init();

    /**
     * 注册
     * @brief regisgerAppender
     * @param appender
     * @param pInterface
     */
    bool regisgerAppender(int appender, Appender *pInterface);
    /**
      * 卸载
      * @brief unregisgerAppender
      * @param appender
      * @return
      */
     Appender *unregisgerAppender(int appender);
     /**
      * 获取Appender
      * @brief getCurrentAppender
      * @return
      */
     Appender *getAppender(int appender);
     /**
      * 获取默认Appender
      * @brief getDefaultAppender
      * @return
      */
     Appender *getDefaultAppender();

    /**
     * 日志后缀
     * @brief setFileSuffix
     * @param suffix
     */
    inline void setFileSuffix(const QString &suffix)
    { m_fileSuffix = suffix; }
    inline const QString &fileSuffix() const
    { return m_fileSuffix; }
    /**
     * 日志路径，默认：当前程序目录
     * @brief setPath
     * @param path
     */
    inline void setPath(const QString &path)
    { m_path = path; }
    inline const QString &path() const
    { return m_path; }
    /**
     * 默认过滤规则，注意：qtlogging.ini 文件不存在时才有用
     * @brief setFilterRules
     * @param reules
     */
    inline void setFilterRules(const QString &reules)
    { m_filterRules = reules; }
    /**
     * 设置消息模式
     * @brief setMessagePattern
     * @param pattern
     */
    inline void setMessagePattern(const QString &pattern)
    { m_messagePattern = pattern; }
    /**
     * 日志输出类型
     * @brief setAppenderName
     * @param type
     */
    inline void setAppenderName(AppenderName name)
    { m_appenderName = name; }
    /**
     * 设置日志文件的最大大小
     * @brief setMaxFileSize
     * @param size
     */
    inline void setMaxFileSize(qint64 size)
    { m_maxFileSize = size; }
    inline qint64 maxFileSize() const
    { return m_maxFileSize; }


private:
    void initLog() ;
    void regisgerDefaultAppender();
    void initLogRules();
    void readSettings();
    bool isDirExists(const QString &dirPath);

    //
    bool openAppender();
    void writeAppender(const QString &s);
    void closeAppender();

private:

#if QT_VERSION >= 0x050000
    static void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#else
    static void myMessageHandler(QtMsgType type, const char* msg);
#endif
    static void myCleanUpRoutine();

private:
    QString m_path;
    QString m_fileSuffix;
    QString m_filterRules;
    QString m_messagePattern;
    int m_appenderName;
    qint64 m_maxFileSize;
    int m_maxBackupIndex;

    QMap<int, Appender*> m_mapAppender;

private:
    explicit MyLog(QObject *parent = NULL);
    ~MyLog();
     static MyLog *m_pLogger;
};

#endif // MYLOG_H
