#include "Logger.h"
#include "appender/Appender.h"
#include "LogManager.h"

Logger::Logger(LoggerRepository* repository, const QString &name, Logger *parent)
    : QObject (parent)
    , m_pLoggerRepository(repository)
    , m_name(name)
    , m_pParent(parent)
{

}
Logger::~Logger()
{

}

Logger *Logger::logger(const QString &rName)
{
    return LogManager::logger(rName);
}


Logger *Logger::logger(const char *pName)
{
    return LogManager::logger(QLatin1String(pName));
}

Logger *Logger::rootLogger()
{
    return LogManager::rootLogger();
}

void Logger::addAppender(Appender *pAppender)
{
    if(!pAppender)
        return;

    if(m_lstAppenders.contains(pAppender))
        return;

    m_lstAppenders.append(pAppender);
}


Appender *Logger::appender(const QString &rName) const
{
    Appender *p_appender;
    Q_FOREACH(p_appender, m_lstAppenders)
        if (p_appender->name() == rName)
            return p_appender;
    return 0;
}

void Logger::removeAllAppenders()
{
    while (!m_lstAppenders.isEmpty())
        delete m_lstAppenders.takeFirst();
}

void Logger::removeAppender(Appender *pAppender)
{
    if(!pAppender)
        return;

    if (!m_lstAppenders.contains(pAppender))
        return;

    m_lstAppenders.removeOne(pAppender);
}


void Logger::removeAppender(const QString &rName)
{
    Appender *p_appender = appender(rName);
    if (p_appender)
        removeAppender(p_appender);
}


void Logger::write(const QString &message)
{
    Appender *p_appender;
    Q_FOREACH(p_appender, m_lstAppenders)
        p_appender->write(message);
}

void Logger::close()
{
    Appender *p_appender;
    Q_FOREACH(p_appender, m_lstAppenders)
        p_appender->close();
}
