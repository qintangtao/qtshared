#ifndef LOGCONFIGURE_H
#define LOGCONFIGURE_H

#include <QObject>


class LogConfigure : public QObject
{
    Q_OBJECT

public:

    enum AppenderName {
        FileAppender = 0,
        ConsoleAppender = 1,
        UserAppender = 2 //用户自定义Appender
    };

    explicit LogConfigure(QObject *parent = Q_NULLPTR);
    ~LogConfigure();

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
    inline const QString &filterRules() const
    { return m_filterRules; }

    /**
     * 设置消息模式
     * @brief setMessagePattern
     * @param pattern
     */
    inline void setMessagePattern(const QString &pattern)
    { m_messagePattern = pattern; }
    inline const QString &messagePattern() const
    { return m_messagePattern; }

    /**
     * 日志输出类型
     * @brief setAppenderName
     * @param type
     */
    inline void setAppenderName(AppenderName name)
    { m_appenderName = name; }
    inline int appenderName() const
    { return m_appenderName; }

    /**
     * 设置日志文件的最大大小
     * @brief setMaxFileSize
     * @param size
     */
    inline void setMaxFileSize(qint64 size)
    { m_maxFileSize = size; }
    inline qint64 maxFileSize() const
    { return m_maxFileSize; }

    /**
     *
     * @brief isDirExists
     * @param dirPath
     * @param created  true:不存在则创建
     * @return
     */
    bool isDirExists(const QString &dirPath, bool created=true);

private:
    void readSettings();

private:
    QString m_path;
    QString m_fileSuffix;
    QString m_filterRules;
    QString m_messagePattern;
    int m_appenderName;
    qint64 m_maxFileSize;
    int m_maxBackupIndex;
};

#endif // LOGCONFIGURE_H
