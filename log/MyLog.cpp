#include "MyLog.h"
#include <QCoreApplication>
#include <QFile>
#include <QMutex>
#include <QDir>
#include <QSettings>
#include <QTextCodec>
#include <QLoggingCategory>
#include "appender/FileAppender.h"
#include "appender/ConsoleAppender.h"
#include "appender/DailyRollingFileAppender.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#endif
#if QT_VERSION < 0x050000
#include "MessagePattern.h"
#endif

#define LOG_CONFIG_FILE_NAME            "qtlogging.ini"
#define LOG_MAX_FILE_SIZE                    4*1024*1024
#define QT_MESSAGE_PATTERN              "[%{time h:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif} %{threadid} %{category}] %{function}:%{line} - %{message}"

#ifdef Q_OS_WIN32

void qOutputDebugPrint(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    QString buf = QString::vasprintf(msg, ap);
    va_end(ap);

    OutputDebugString(reinterpret_cast<const wchar_t *>(buf.utf16()));
}

void qOutputDebugPrintError(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    QString buf = QString::vasprintf(msg, ap);
    va_end(ap);

    buf += QLatin1String(" (") + qt_error_string(-1) + QLatin1Char(')');

    OutputDebugString(reinterpret_cast<const wchar_t *>(buf.utf16()));
}

#endif

MyLog::MyLog(QObject *parent)
    : QObject(parent)
    , m_appenderName(AppenderName::FileAppender)
    , m_messagePattern(QT_MESSAGE_PATTERN)
    , m_fileSuffix("log")
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
    qOutputDebugPrint(m_path.toLocal8Bit().constData());

    readSettings();
    initLogRules();
    regisgerDefaultAppender();
}

MyLog::~MyLog()
{

}

MyLog *MyLog::m_pLogger = NULL;
MyLog *MyLog::Logger()
{
       if (m_pLogger == Q_NULLPTR)
           m_pLogger = new MyLog();
       return m_pLogger;
}

void MyLog::init()
{
    Logger()->initLog();
}

void MyLog::initLog()
{
    qSetMessagePattern(m_messagePattern);
#  if QT_VERSION >= 0x050000
    qInstallMessageHandler(MyLog::myMessageHandler);
#  else
    qInstallMsgHandler(MyLog::myMessageHandler);
#  endif
    qAddPostRoutine(MyLog::myCleanUpRoutine);

    openAppender();
}

bool  MyLog::regisgerAppender(int appender, Appender *pInterface)
{
    if (NULL == pInterface)
        return false;

    if (m_mapAppender.contains(appender))
        return false;

    m_mapAppender.insert(appender, pInterface);

    return true;
}

Appender *MyLog::unregisgerAppender(int appender)
{
    if (m_mapAppender.contains(appender))
        return m_mapAppender.take(appender);

    return NULL;
}

Appender *MyLog::getAppender(int appender)
{
    if (m_mapAppender.contains(appender))
        return m_mapAppender.value(appender);

    return NULL;
}

Appender *MyLog::getDefaultAppender()
{
    Appender *pInterface = getAppender(m_appenderName);
    if (NULL == pInterface) {
    #ifdef Q_OS_WIN32
            qOutputDebugPrint("MyLog::getDefaultAppender %d appender is no registration.", m_appenderName);
    #else
            fprintf(stderr, "MyLog::getDefaultAppender %d appender is no registration.",  m_appenderName);
    #endif
   }
    return pInterface;
}

void MyLog::regisgerDefaultAppender()
{
    DailyRollingFileAppender *a = new DailyRollingFileAppender(this);
    a->setFile("F:\\work\\kang\\KangView-control\\bin\\aaa.log");
    a->setAppendFile(true);
    a->setDatePattern(DailyRollingFileAppender::MINUTELY_ROLLOVER);
    //a = new class FileAppender(this);

    regisgerAppender(AppenderName::FileAppender, a);
    regisgerAppender(AppenderName::ConsoleAppender, new class ConsoleAppender(this));

    //m_mapAppender.insert(AppenderName::FileAppender, new class FileAppender(this));
    //m_mapAppender.insert(AppenderName::ConsoleAppender, new class ConsoleAppender(this));
}

void MyLog::initLogRules()
{
    if (QFile::exists(LOG_CONFIG_FILE_NAME)) {
        qputenv("QT_LOGGING_CONF", LOG_CONFIG_FILE_NAME);
    } else {
        QLoggingCategory::setFilterRules(m_filterRules);
    }
}

void MyLog::readSettings()
{
    QSettings s(LOG_CONFIG_FILE_NAME , QSettings::IniFormat, this);
    s.setIniCodec(QTextCodec::codecForName("utf-8"));
    QString path = s.value("/settings/path").toString();
    QString suffix = s.value("/settings/suffix").toString();
    QString rules = s.value("/settings/rules").toString();
    QString messagePattern = s.value("/settings/messagePattern").toString();
    QString appenderName = s.value("/settings/appenderName").toString();
    qint64 maxFileSize = s.value("/settings/maxFileSize").toInt();

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

bool MyLog::isDirExists(const QString &dirPath)
{
    QDir dir(dirPath);
    if (dir.exists()) {
        return true;
    }
    return dir.mkpath(dirPath);
}

bool MyLog::openAppender()
{
    Appender *pInterface = getDefaultAppender();
    if (NULL != pInterface) {
        pInterface->open();
    }
    return false;
}

void MyLog::writeAppender(const QString &s)
{
    Appender *pInterface = getDefaultAppender();
    if (NULL != pInterface) {
        pInterface->write(s);
    }
}

void MyLog::closeAppender()
{
    Appender *pInterface = getDefaultAppender();
    if (NULL != pInterface) {
        pInterface->close();
    }
}

#if QT_VERSION >= 0x050000
void MyLog::myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#else
void MyLog::myMessageHandler(QtMsgType type, const char* msg)
#endif
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    QString s;
#if QT_VERSION >= 0x050000
    s.append(qFormatLogMessage(type, context, msg));
#else
    s.append(qFormatLogMessage(type, msg));
#endif
    s.append(QLatin1Char('\n'));

    QString category = context.category;

    MyLog *logger = MyLog::Logger();
    logger->writeAppender(s);
}

void MyLog::myCleanUpRoutine()
{
    MyLog *logger = MyLog::Logger();
    logger->closeAppender();
}

