#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QAtomicPointer>
#include "../helpers/ClassLogger.h"

#if QT_VERSION < QT_VERSION_CHECK(4, 4, 0)
    #define LOG4QT_DECLARE_STATIC_LOGGER(FUNCTION, CLASS)                     \
        static Logger *FUNCTION()                                     \
        {                                                                     \
            static Logger *p_logger = 0;                              \
                if (!p_logger)                                                \
                {                                                             \
                    q_atomic_test_and_set_ptr(                                \
                        &p_logger,                                            \
                        0,                                                    \
                        Logger::logger( #CLASS ));                    \
                }                                                             \
            return p_logger;                                                  \
        }
#else
    #define LOG4QT_DECLARE_STATIC_LOGGER(FUNCTION, CLASS)                     \
        static Logger *FUNCTION()                                     \
        {                                                                     \
            static QBasicAtomicPointer<Logger > p_logger =            \
                Q_BASIC_ATOMIC_INITIALIZER(0);                                \
            if (!p_logger)                                                    \
            {                                                                 \
                p_logger.testAndSetOrdered(0,                                 \
                    Logger::logger( #CLASS ));                        \
            }                                                                 \
            return p_logger;                                                  \
        }
#endif

#define LOG4QT_DECLARE_QCLASS_LOGGER                                      \
    private:                                                              \
        mutable ClassLogger mLog4QtClassLogger;                   \
    public:                                                               \
        inline Logger *logger() const                             \
        {   return mLog4QtClassLogger.logger(this);    }                  \
    private:

class Appender;
class LoggerRepository;

class Logger : public QObject
{
    Q_OBJECT

    LOG4QT_DECLARE_QCLASS_LOGGER

public:
        Logger(LoggerRepository* repository, const QString &name, Logger *parent = 0);
        virtual ~Logger();
private:
    Logger(const Logger &rOther); // Not implemented
    Logger &operator=(const Logger &rOther); // Not implemented

public:
    static Logger *logger(const QString &rName);
    static Logger *logger(const char *pName);
    static Logger *rootLogger();

    void addAppender(Appender *pAppender);
    Appender *appender(const QString &rName) const;
    void removeAllAppenders();
    void removeAppender(Appender *pAppender);
    void removeAppender(const QString &rName);

    void write(const QString &message);
    void close();

private:
    const QString m_name;
    LoggerRepository* m_pLoggerRepository;
    Logger *m_pParent;
    QList<Appender *> m_lstAppenders;
};


#endif // LOGGER_H
