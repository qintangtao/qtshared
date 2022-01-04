#ifndef FACTORY_H
#define FACTORY_H
#include <QObject>
#include <QHash>

class Appender;

class Factory
{
public:
    typedef Appender *(*AppenderFactoryFunc)();

private:
    Factory();
    Factory(const Factory &rOther); // Not implemented
    // virtual ~Factory(); // Use compiler default
    Factory &operator=(const Factory &rOther); // Not implemented

public:
    static Appender *createAppender(const QString &rAppenderClassName);
    static Appender *createAppender(const char *pAppenderClassName);

    static Factory *instance();

    static void registerAppender(const QString &rAppenderClassName,
                                 AppenderFactoryFunc pAppenderFactoryFunc);

    static void registerAppender(const char *pAppenderClassName,
                                 AppenderFactoryFunc pAppenderFactoryFunc);

    static QStringList registeredAppenders();

    static void setObjectProperty(QObject *pObject,
                                  const QString &rProperty,
                                  const QString &rValue);

    static void setObjectProperty(QObject *pObject,
                                  const char *pProperty,
                                  const QString &rValue);

    static void unregisterAppender(const QString &rAppenderClassName);

    static void unregisterAppender(const char *pAppenderClassName);

private:
    Appender *doCreateAppender(const QString &rAppenderClassName);
    void doRegisterAppender(const QString &rAppenderClassName,
                            AppenderFactoryFunc pAppenderFactoryFunc);
    void doSetObjectProperty(QObject *pObject,
                             const QString &rProperty,
                             const QString &rValue);
    void doUnregisterAppender(const QString &rAppenderClassName);
    void registerDefaultAppenders();
    bool validateObjectProperty(QMetaProperty &rMetaProperty,
                                const QString &rProperty,
                                QObject *pObject);

private:
    QHash<QString, AppenderFactoryFunc> m_appenderRegistry;
};

inline Appender *Factory::createAppender(const QString &rAppenderClassName)
{   return instance()->doCreateAppender(rAppenderClassName);    }

inline Appender *Factory::createAppender(const char *pAppenderClassName)
{	return instance()->doCreateAppender(QLatin1String(pAppenderClassName));	}

inline void Factory::registerAppender(const QString &rAppenderClassName,
                                      AppenderFactoryFunc pAppenderFactoryFunc)
{   instance()->doRegisterAppender(rAppenderClassName, pAppenderFactoryFunc);  }

inline void Factory::registerAppender(const char *pAppenderClassName,
                                      AppenderFactoryFunc pAppenderFactoryFunc)
{	instance()->doRegisterAppender(QLatin1String(pAppenderClassName), pAppenderFactoryFunc);	}

inline QStringList Factory::registeredAppenders()
{	return instance()->m_appenderRegistry.keys();	}

inline void Factory::setObjectProperty(QObject *pObject,
                                       const QString &rProperty,
                                       const QString &rValue)
{   instance()->doSetObjectProperty(pObject, rProperty, rValue); }

inline void Factory::setObjectProperty(QObject *pObject,
                                       const char *pProperty,
                                       const QString &rValue)
{   instance()->doSetObjectProperty(pObject, QLatin1String(pProperty), rValue);  }

inline void Factory::unregisterAppender(const QString &rAppenderClassName)
{   instance()->doUnregisterAppender(rAppenderClassName);  }

inline void Factory::unregisterAppender(const char *pAppenderClassName)
{	instance()->doUnregisterAppender(QLatin1String(pAppenderClassName));	}

#endif // FACTORY_H
