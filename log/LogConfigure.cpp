#include "LogConfigure.h"
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QTextCodec>

#define LOG_MAX_FILE_SIZE                    4*1024*1024
#define LOG_CONFIG_FILE_NAME            "qtlogging.ini"
#define LOG_FILE_SUFFIX                         "log"
#define LOG_FILTER_RULES                      "*=false"
#define LOG_MESSAGE_PATTERN             "[%{time h:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif} %{threadid} %{category}] %{function}:%{line} - %{message}"

#ifdef Q_OS_WIN32
extern void qOutputDebugPrint(const char *msg, ...);
extern void qOutputDebugPrintError(const char *msg, ...);
#endif

LogConfigure::LogConfigure(QObject *parent)
      : QObject(parent)
      , m_appenderName(AppenderName::FileAppender)
      , m_messagePattern(LOG_MESSAGE_PATTERN)
      , m_fileSuffix(LOG_FILE_SUFFIX)
      , m_filterRules("*=false")
      , m_maxBackupIndex (10)
      , m_maxFileSize(LOG_MAX_FILE_SIZE)
{
    #if defined(Q_OS_WIN32)
        QDir dir(QCoreApplication::applicationDirPath());
        m_path = dir.absoluteFilePath("log");
    #else
        m_path = "/tmp";
    #endif
     isDirExists(m_path);
    //qOutputDebugPrint(m_path.toLocal8Bit().constData());

     readSettings();
}

LogConfigure::~LogConfigure()
{

}

bool LogConfigure::isDirExists(const QString &dirPath, bool created)
{
    QDir dir(dirPath);
    if (dir.exists())
        return true;

    if (created)
        return dir.mkpath(dirPath);
    return false;
}

void LogConfigure::readSettings()
{
    QSettings s(LOG_CONFIG_FILE_NAME , QSettings::IniFormat, this);
    s.setIniCodec(QTextCodec::codecForName("utf-8"));
    QString path = s.value("/settings/path").toString();
    QString suffix = s.value("/settings/suffix").toString();
    QString rules = s.value("/settings/rules").toString();
    QString messagePattern = s.value("/settings/messagePattern").toString();
    QString appenderName = s.value("/settings/appenderName").toString();
    qint64 maxFileSize = s.value("/settings/maxFileSize").toInt();

    printf("MyLog::readSettings\n");

#ifdef Q_OS_WIN32
    qOutputDebugPrint("MyLog::readSettings\n");
    qOutputDebugPrint("path:%s\n", path.toLocal8Bit().constData());
    qOutputDebugPrint("suffix:%s\n", suffix.toLocal8Bit().constData());
    qOutputDebugPrint("rules:%s\n", rules.toLocal8Bit().constData());
    qOutputDebugPrint("messagePattern:%s\n", messagePattern.toLocal8Bit().constData());
    qOutputDebugPrint("appenderName:%s\n", appenderName.toLocal8Bit().constData());
    qOutputDebugPrint("maxFileSize:%d\n", maxFileSize);
#else
    printf("MyLog::readSettings\n");
    fprintf(stdout, "path:%s\n", path.toLocal8Bit().constData());
    fprintf(stdout, "suffix:%s\n", suffix.toLocal8Bit().constData());
    fprintf(stdout, "rules:%s\n", rules.toLocal8Bit().constData());
    fprintf(stdout, "messagePattern:%s\n", messagePattern.toLocal8Bit().constData());
    fprintf(stdout, "appenderName:%s\n", appenderName.toLocal8Bit().constData());
    fprintf(stdout, "maxFileSize:%d\n", maxFileSize);
#endif

    if (!path.isEmpty() && isDirExists(path)) {
        m_path = path;
    }
    if (!suffix.isEmpty()) {
        m_fileSuffix = suffix;
    }
    if(!rules.isEmpty()) {
        m_filterRules = rules;
    }
    if(!messagePattern.isEmpty()) {
        m_messagePattern = messagePattern;
    }
    if (!appenderName.isEmpty()) {
        if (appenderName == "FileAppender") {
            m_appenderName = AppenderName::FileAppender;
        } else if (appenderName == "ConsoleAppender") {
            m_appenderName = AppenderName::ConsoleAppender;
        } else {
            bool ok = false;
            int appender = appenderName.toInt(&ok);
            if (ok) {
                m_appenderName = appender;
            }
        }
    }
    if (maxFileSize >= 1024 && maxFileSize < INT64_MAX) {
        m_maxFileSize = maxFileSize;
    }
}
