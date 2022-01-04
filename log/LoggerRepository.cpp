#include "LoggerRepository.h"
#include "Logger.h"


LoggerRepository::LoggerRepository() :
    m_pRootLogger(logger(QString()))
{

}
LoggerRepository::~LoggerRepository()
{

}

bool LoggerRepository::exists(const QString &rName) const
{
    return m_loggers.contains(rName);
}
Logger *LoggerRepository::logger(const QString &rName)
{
    return createLogger(rName);
}
QList<Logger *> LoggerRepository::loggers() const
{
    return m_loggers.values();
}

void LoggerRepository::resetConfiguration()
{
    Logger *p_logging_logger = logger(QLatin1String("Log4Qt"));
    Logger *p_qt_logger = logger(QLatin1String("Qt"));
    Logger *p_root_logger = rootLogger();

    Logger *p_logger;
    Q_FOREACH(p_logger, m_loggers)
    {
        if ((p_logger == p_logging_logger) || (p_logger == p_qt_logger) || (p_logger == p_root_logger))
            continue;
        resetLogger(p_logger);
    }
    resetLogger(p_qt_logger);
    resetLogger(p_logging_logger);
    resetLogger(p_root_logger);
}

void LoggerRepository::shutdown()
{
    resetConfiguration();
}

Logger *LoggerRepository::createLogger(const QString &rName)
{
    const QString name_separator = QLatin1String("::");

    Logger *p_logger = m_loggers.value(rName, 0);
    if (p_logger != 0)
        return p_logger;

    if (rName.isEmpty())
    {
        p_logger = new Logger(this, QLatin1String("root"), 0);
        m_loggers.insert(QString(), p_logger);
        return p_logger;
    }
    QString parent_name;
    int index = rName.lastIndexOf(name_separator);
    if (index >=0)
        parent_name = rName.left(index);
    p_logger = new Logger(this, rName, createLogger(parent_name));
    m_loggers.insert(rName, p_logger);
    return p_logger;
}

void LoggerRepository::resetLogger(Logger *pLogger) const
{
    pLogger->removeAllAppenders();
}
