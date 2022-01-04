#ifndef LOGGERREPOSITORY_H
#define LOGGERREPOSITORY_H

#include <QHash>

class Logger;

class LoggerRepository
{
public:
    LoggerRepository();
    ~LoggerRepository();

public:
    bool exists(const QString &rName) const;
    Logger *logger(const QString &rName);
    QList<Logger *> loggers() const;
    Logger *rootLogger() const;
    void resetConfiguration();
    void shutdown();

private:
   Logger *createLogger(const QString &rName);
   void resetLogger(Logger *pLogger) const;

private:
    QHash<QString, Logger*> m_loggers;
    Logger *m_pRootLogger;
};

inline Logger *LoggerRepository::rootLogger() const
{   return m_pRootLogger;    }

#endif // LOGGERREPOSITORY_H
