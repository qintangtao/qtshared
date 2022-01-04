#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QtGlobal>
#include <QObject>
#include <QList>

class  LoggerRepository;
class Logger;

class LogManager
{
private:
    LogManager();
    LogManager(const LogManager &rOther); // Not implemented
    virtual ~LogManager();
    LogManager &operator=(const LogManager &rOther); // Not implemented

public:

    static bool handleQtMessages();

    static LoggerRepository *loggerRepository();

    static Logger *logLogger();

    static Logger *qtLogger();

    static Logger *rootLogger();
    static QList<Logger*> loggers();

    static void setMessagePattern(const QString &pattern);

    static void setHandleQtMessages(bool handleQtMessages);
    static void configureLogLogger();

    static bool exists(const QString &name);
    static LogManager *instance();

    static Logger *logger(const QString &rName);

    static void resetConfiguration();

    static void shutdown();

    static void startup();

    static const char* version();

private:
    void doSetHandleQtMessages(bool handleQtMessages);
    void doConfigureLogLogger();
    void doStartup();
    void welcome();

#if QT_VERSION >= 0x050000
    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#else
    static void qtMessageHandler(QtMsgType type, const char* msg);
#endif

private:
    LoggerRepository *mpLoggerRepository;
    Logger *mpNullLogger;
    bool mHandleQtMessages;
#if QT_VERSION >= 0x050000
    QtMessageHandler mOldQtMsgHandler;
#else
    QtMsgHandler mOldQtMsgHandler;
#endif
    static LogManager *mspInstance;
};

inline LoggerRepository *LogManager::loggerRepository()
{   return instance()->mpLoggerRepository;  }

inline bool LogManager::handleQtMessages()
{   return instance()->mHandleQtMessages;   }

inline Logger *LogManager::logLogger()
{  return logger(QLatin1String("Log4Qt"));   }

inline Logger *LogManager::qtLogger()
{   return logger(QLatin1String("Qt"));    }

inline void LogManager::setHandleQtMessages(bool handleQtMessages)
{	instance()->doSetHandleQtMessages(handleQtMessages);	}

inline void LogManager::configureLogLogger()
{	instance()->doConfigureLogLogger();	}

inline void LogManager::startup()
{	instance()->doStartup();	}


#endif // LOGMANAGER_H
