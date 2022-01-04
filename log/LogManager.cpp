#include "LogManager.h"
#include <QCoreApplication>
#include <QMutexLocker>
#include "appender/Appender.h"
#include "LoggerRepository.h"
#include "Logger.h"
#include "LogDef.h"
#include "../helpers/OptionConverter.h"

#define LOG_CONFIG_FILE_NAME            "qtlogging.ini"
#define QT_MESSAGE_PATTERN              "[%{time h:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif} %{threadid} %{category}] %{function}:%{line} - %{message}"


LogManager::LogManager() :
    mpLoggerRepository(new LoggerRepository()),
    mHandleQtMessages(false),
    mOldQtMsgHandler(0)
{
    if (QFile::exists(LOG_CONFIG_FILE_NAME)) {
        qputenv("QT_LOGGING_CONF", LOG_CONFIG_FILE_NAME);
     }
    setMessagePattern(QT_MESSAGE_PATTERN);
}

LogManager::~LogManager()
{
}

Logger *LogManager::rootLogger()
{
    return instance()->mpLoggerRepository->rootLogger();
}

QList<Logger*> LogManager::loggers()
{
    return instance()->mpLoggerRepository->loggers();
}

bool LogManager::exists(const QString &name)
{
    return instance()->mpLoggerRepository->exists(name);
}

void LogManager::setMessagePattern(const QString &pattern)
{
    if (pattern.isEmpty())
        return;

    qSetMessagePattern(pattern);
}

LogManager *LogManager::instance()
{
    if (!mspInstance)
    {
        if (!mspInstance)
        {
            mspInstance = new LogManager;
            // qAddPostRoutine(shutdown);
            atexit(shutdown);
            mspInstance->doSetHandleQtMessages(true);
            mspInstance->doConfigureLogLogger();
            mspInstance->welcome();
            mspInstance->doStartup();
        }
    }
    return mspInstance;
}

Logger *LogManager::logger(const QString &rName)
{
    return instance()->mpLoggerRepository->logger(rName);
}

void LogManager::resetConfiguration()
{
    setHandleQtMessages(false);
    instance()->mpLoggerRepository->resetConfiguration();
    configureLogLogger();
}

const char* LogManager::version()
{
    return LOG_VERSION_STR;
}

void LogManager::shutdown()
{
    instance()->mpLoggerRepository->shutdown();
}

void LogManager::doSetHandleQtMessages(bool handleQtMessages)
{
    if (instance()->mHandleQtMessages == handleQtMessages)
        return;

    instance()->mHandleQtMessages = handleQtMessages;
    if (instance()->mHandleQtMessages)
    {
#  if QT_VERSION >= 0x050000
        instance()->mOldQtMsgHandler = qInstallMessageHandler(qtMessageHandler);
#  else
        instance()->mOldQtMsgHandler = qInstallMsgHandler(qtMessageHandler);
#  endif
    }
    else
    {
#  if QT_VERSION >= 0x050000
        qInstallMessageHandler(instance()->mOldQtMsgHandler);
#  else
        qInstallMsgHandler(instance()->mOldQtMsgHandler);
#  endif
    }
}

void LogManager::doConfigureLogLogger()
{
#if 0
    QMutexLocker locker(&instance()->mObjectGuard);

    // Level
    QString value = InitialisationHelper::setting(QLatin1String("Debug"),
                                                  QLatin1String("ERROR"));
    logLogger()->setLevel(OptionConverter::toLevel(value, Level::DEBUG_INT));

    // Common layout
    TTCCLayout *p_layout = new TTCCLayout();
    p_layout->setName(QLatin1String("LogLog TTCC"));
    p_layout->setContextPrinting(false);
    p_layout->activateOptions();

    // Common deny all filter
    Filter *p_denyall = new DenyAllFilter();
    p_denyall->activateOptions();

    // ConsoleAppender on stdout for all events <= INFO
    ConsoleAppender *p_appender;
    LevelRangeFilter *p_filter;
    p_appender = new ConsoleAppender(p_layout, ConsoleAppender::STDOUT_TARGET);
    p_filter = new LevelRangeFilter();
    p_filter->setNext(p_denyall);
    p_filter->setLevelMin(Level::NULL_INT);
    p_filter->setLevelMax(Level::INFO_INT);
    p_filter->activateOptions();
    p_appender->setName(QLatin1String("LogLog stdout"));
    p_appender->addFilter(p_filter);
    p_appender->activateOptions();
    logLogger()->addAppender(p_appender);

    // ConsoleAppender on stderr for all events >= WARN
    p_appender = new ConsoleAppender(p_layout, ConsoleAppender::STDERR_TARGET);
    p_filter = new LevelRangeFilter();
    p_filter->setNext(p_denyall);
    p_filter->setLevelMin(Level::WARN_INT);
    p_filter->setLevelMax(Level::OFF_INT);
    p_filter->activateOptions();
    p_appender->setName(QLatin1String("LogLog stderr"));
    p_appender->addFilter(p_filter);
    p_appender->activateOptions();
    logLogger()->addAppender(p_appender);
#endif
}

void LogManager::doStartup()
{
#if 0
    // Override
    QString default_value = QLatin1String("false");
    QString value = InitialisationHelper::setting(QLatin1String("DefaultInitOverride"),
                                                  default_value);
    if (value != default_value)
    {
        static_logger()->debug("DefaultInitOverride is set. Aborting default initialisation");
        return;
    }

    // Configuration using setting Configuration
    value = InitialisationHelper::setting(QLatin1String("Configuration"));
    if (QFile::exists(value))
    {
        static_logger()->debug("Default initialisation configures from file '%1' specified by Configure", value);
        PropertyConfigurator::configure(value);
        return;
    }

    // Configuration using setting
    if (QCoreApplication::instance())
    {
        const QLatin1String log4qt_group("Log4Qt");
        const QLatin1String properties_group("Properties");
        QSettings s;
        s.beginGroup(log4qt_group);
        if (s.childGroups().contains(properties_group))
        {
            const QString group(QLatin1String("Log4Qt/Properties"));
            static_logger()->debug("Default initialisation configures from setting '%1/%2'", log4qt_group, properties_group);
            s.beginGroup(properties_group);
            PropertyConfigurator::configure(s);
            return;
        }
    }

    // Configuration using default file
    const QString default_file(QLatin1String("log4qt.properties"));
    if (QFile::exists(default_file))
    {
        static_logger()->debug("Default initialisation configures from default file '%1'", default_file);
        PropertyConfigurator::configure(default_file);
        return;
    }

    static_logger()->debug("Default initialisation leaves package unconfigured");
#endif
}

void LogManager::welcome()
{
#if 0
    static_logger()->info("Initialising Log4Qt %1",
                          QLatin1String(LOG4QT_VERSION_STR));

    // Debug: Info
    if (static_logger()->isDebugEnabled())
    {
        // Create a nice timestamp with UTC offset
        DateTime start_time = DateTime::fromMilliSeconds(InitialisationHelper::startTime());
        QString offset;
        {
            QDateTime utc = start_time.toUTC();
            QDateTime local = start_time.toLocalTime();
            QDateTime local_as_utc = QDateTime(local.date(), local.time(), Qt::UTC);
            int min = utc.secsTo(local_as_utc) / 60;
            if (min < 0)
                offset += QLatin1Char('-');
            else
                offset += QLatin1Char('+');
            min = abs(min);
            offset += QString::number(min / 60).rightJustified(2, QLatin1Char('0'));
            offset += QLatin1Char(':');
            offset += QString::number(min % 60).rightJustified(2, QLatin1Char('0'));
        }
        static_logger()->debug("Program startup time is %1 (UTC%2)",
                               start_time.toString(QLatin1String("ISO8601")),
                               offset);
        static_logger()->debug("Internal logging uses the level %1",
                               logLogger()->level().toString());
    }

    // Trace: Dump settings
    if (static_logger()->isTraceEnabled())
    {
        static_logger()->trace("Settings from the system environment:");
        QString entry;
        Q_FOREACH (entry, InitialisationHelper::environmentSettings().keys())
            static_logger()->trace("    %1: '%2'",
                                   entry,
                                   InitialisationHelper::environmentSettings().value(entry));

        static_logger()->trace("Settings from the application settings:");
        if (QCoreApplication::instance())
        {
            const QLatin1String log4qt_group("Log4Qt");
            const QLatin1String properties_group("Properties");
            static_logger()->trace("    %1:", log4qt_group);
            QSettings s;
            s.beginGroup(log4qt_group);
            Q_FOREACH (entry, s.childKeys())
                static_logger()->trace("        %1: '%2'",
                                       entry,
                                       s.value(entry).toString());
            static_logger()->trace("    %1/%2:", log4qt_group, properties_group);
            s.beginGroup(properties_group);
                Q_FOREACH (entry, s.childKeys())
                    static_logger()->trace("        %1: '%2'",
                                           entry,
                                           s.value(entry).toString());
        } else
            static_logger()->trace("    QCoreApplication::instance() is not available");
    }
#endif
}

#if QT_VERSION >= 0x050000
void LogManager::qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
#else
void LogManager::qtMessageHandler(QtMsgType type, const char* msg)
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

#if QT_VERSION >= 0x050000
    QString cpp_name = OptionConverter::classNameJavaToCpp(context.category);
    Logger *logger = NULL;
    if (instance()->exists(cpp_name)) {
        logger = instance()->logger(cpp_name);
    } else {
        logger = instance()->rootLogger();
    }
#else
    logger = instance()->rootLogger();
#endif
    logger->write(s);
}

#if 0
void LogManager::qtMessageHandler(QtMsgType type, const char *pMessage)
{
    Level level;
    switch (type)
    {
        case QtDebugMsg:
            level = Level::DEBUG_INT;
            break;
        case QtWarningMsg:
            level = Level::WARN_INT;
            break;
        case QtCriticalMsg:
            level = Level::ERROR_INT;
            break;
        case QtFatalMsg:
            level = Level::FATAL_INT;
            break;
        default:
            level = Level::TRACE_INT;
    }
    instance()->qtLogger()->log(level, pMessage);

    // Qt fatal behaviour copied from global.cpp qt_message_output()
    // begin {

    if ((type == QtFatalMsg) ||
        ((type == QtWarningMsg) && (!qgetenv("QT_FATAL_WARNINGS").isNull())) )
    {
#if defined(Q_CC_MSVC) && defined(QT_DEBUG) && defined(_DEBUG) && defined(_CRT_ERROR)
        // get the current report mode
        int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
        _CrtSetReportMode(_CRT_ERROR, reportMode);
        int ret = _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, QT_VERSION_STR, pMessage);
        if (ret == 0  && reportMode & _CRTDBG_MODE_WNDW)
            return; // ignore
        else if (ret == 1)
            _CrtDbgBreak();
#endif

#if defined(Q_OS_UNIX) && defined(QT_DEBUG)
        abort(); // trap; generates core dump
#else
        exit(1); // goodbye cruel world
#endif
    }

    // } end
}
#endif

LogManager *LogManager::mspInstance = 0;
