#include "Factory.h"
#include <QMetaProperty>
#include <QVariant>
#include "OptionConverter.h"
#include "../LogDef.h"
#include "../appender/NullAppender.h"
#include "../appender/ListAppender.h"
#include "../appender/DebugAppender.h"
#include "../appender/ConsoleAppender.h"
#include "../appender/FileAppender.h"
#include "../appender/RollingFileAppender.h"
#include "../appender/DailyRollingFileAppender.h"

Appender *console_file_appender()
{	return new ConsoleAppender(0);	}

Appender *create_daily_rolling_file_appender()
{	return new DailyRollingFileAppender(0);	}

Appender *create_debug_appender()
{	return new DebugAppender(0);	}

Appender *create_file_appender()
{	return new FileAppender(0);	}

Appender *create_list_appender()
{	return new ListAppender(0);	}

Appender *create_null_appender()
{	return new NullAppender(0);	}

Appender *create_rolling_file_appender()
{	return new RollingFileAppender(0);	}

Factory::Factory()
{
    registerDefaultAppenders();
}

LOG_IMPLEMENT_INSTANCE(Factory)

Appender *Factory::doCreateAppender(const QString &rAppenderClassName)
{
    if (!m_appenderRegistry.contains(rAppenderClassName))
    {
        return 0;
    }
    return m_appenderRegistry.value(rAppenderClassName)();
}

void Factory::doRegisterAppender(const QString &rAppenderClassName,
                                 AppenderFactoryFunc pAppenderFactoryFunc)
{
    if(rAppenderClassName.isEmpty())
    {
        return;
    }
    m_appenderRegistry.insert(rAppenderClassName, pAppenderFactoryFunc);
}


void Factory::doSetObjectProperty(QObject *pObject,
                                  const QString &rProperty,
                                  const QString &rValue)
{
    QMetaProperty meta_property;
    if (!validateObjectProperty(meta_property, rProperty, pObject))
        return;

    QString property = QLatin1String(meta_property.name());
    QString type = QLatin1String(meta_property.typeName());

    QVariant value;
    bool ok = true;
    if (type == QLatin1String("bool"))
        value = OptionConverter::toBoolean(rValue, &ok);
    else if (type == QLatin1String("int"))
        value = OptionConverter::toInt(rValue, &ok);
    else if (type == QLatin1String("QString"))
        value = rValue;
    else
    {
        return;
    }

    if (!ok)
        return;

    meta_property.write(pObject, value);
}


void Factory::doUnregisterAppender(const QString &rAppenderClassName)
{
    if (!m_appenderRegistry.contains(rAppenderClassName))
    {
        return;
    }
    m_appenderRegistry.remove(rAppenderClassName);
}

void Factory::registerDefaultAppenders()
{
    m_appenderRegistry.insert(QLatin1String("ListAppender"), create_list_appender);
    m_appenderRegistry.insert(QLatin1String("NullAppender"), create_null_appender);
    m_appenderRegistry.insert(QLatin1String("DebugAppender"), create_debug_appender);
    m_appenderRegistry.insert(QLatin1String("ConsoleAppender"), console_file_appender);
    m_appenderRegistry.insert(QLatin1String("FileAppender"), create_file_appender);
    m_appenderRegistry.insert(QLatin1String("RollingFileAppender"), create_rolling_file_appender);
    m_appenderRegistry.insert(QLatin1String("DailyRollingFileAppender"), create_daily_rolling_file_appender);
}

bool Factory::validateObjectProperty(QMetaProperty &rMetaProperty,
                                     const QString &rProperty,
                                     QObject *pObject)
{
    if (!pObject)
    {
        return false;
    }
    if (rProperty.isEmpty())
    {
        return false;
    }
    const QMetaObject *p_meta_object = pObject->metaObject();
    QString property = rProperty;
    int i = p_meta_object->indexOfProperty(property.toLatin1());
    if (i < 0)
    {
        property[0] = property[0].toLower();
        i = p_meta_object->indexOfProperty(property.toLatin1());
        if (i < 0)
        {
            return false;
        }
    }
    rMetaProperty = p_meta_object->property(i);
    if (!rMetaProperty.isWritable())
    {
        return false;
    }

    return true;
}
