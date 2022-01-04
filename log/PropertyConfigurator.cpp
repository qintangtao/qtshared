#include "PropertyConfigurator.h"
#include <QFile>
#include "helpers/OptionConverter.h"
#include "helpers/Factory.h"
#include "appender/Appender.h"
#include "appender/ListAppender.h"
#include "Logger.h"
#include "LoggerRepository.h"
#include "LogManager.h"


LOG4QT_DECLARE_STATIC_LOGGER(logger, PropertyConfigurator)

bool PropertyConfigurator::configure(const Properties &rProperties)
{
    PropertyConfigurator configurator;
    return configurator.doConfigure(rProperties);
}
bool PropertyConfigurator::configure(const QString &rConfigFilename)
{
    PropertyConfigurator configurator;
    return configurator.doConfigure(rConfigFilename);
}
bool PropertyConfigurator::configure(const QSettings &rSettings)
{
    PropertyConfigurator configurator;
    return configurator.doConfigure(rSettings);
}

bool PropertyConfigurator::doConfigure(const Properties &rProperties,
                                       LoggerRepository *pLoggerRepository)
{
    startCaptureErrors();
    configureFromProperties(rProperties, pLoggerRepository);
    return stopCaptureErrors();
}
bool PropertyConfigurator::doConfigure(const QString &rConfigFileName,
                                       LoggerRepository *pLoggerRepository)
{
    startCaptureErrors();
    configureFromFile(rConfigFileName, pLoggerRepository);
    return stopCaptureErrors();
}

bool PropertyConfigurator::doConfigure(const QSettings &rSettings,
                                       LoggerRepository *pLoggerRepository)
{
    startCaptureErrors();
    configureFromSettings(rSettings, pLoggerRepository);
    return stopCaptureErrors();
}


void PropertyConfigurator::configureFromProperties(const Properties &rProperties,
                                                   LoggerRepository *pLoggerRepository)
{
    if (!pLoggerRepository)
        pLoggerRepository = LogManager::loggerRepository();

    configureGlobalSettings(rProperties, pLoggerRepository);
    configureRootLogger(rProperties, pLoggerRepository);
    configureNonRootElements(rProperties, pLoggerRepository);
    m_hashAppenderRegistry.clear();
}

void PropertyConfigurator::configureFromFile(const QString &rConfigFileName,
                                             LoggerRepository *pLoggerRepository)
{
    QFile file(rConfigFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logger()->write(QString("Unable to open property file '%1'").arg(rConfigFileName));
        return;
    }

    Properties properties;
    properties.load(&file);
    if (file.error())  {
        logger()->write(QString("Unable to read property file '%1'").arg(rConfigFileName));
        return;
    }

    configureFromProperties(properties, pLoggerRepository);
}

void PropertyConfigurator::configureFromSettings(const QSettings &rSettings,
                                                 LoggerRepository *pLoggerRepository)
{
    Properties properties;
    properties.load(rSettings);
    configureFromProperties(properties, pLoggerRepository);
}

void PropertyConfigurator::configureGlobalSettings(const Properties &rProperties,
                                     LoggerRepository *pLoggerRepository) const
{
    Q_ASSERT_X(pLoggerRepository, "PropertyConfigurator::configureRootLogger()", "pLoggerRepository must not be null.");

    const QLatin1String key_message_pattern("log.messagePattern");
    const QLatin1String key_handle_qt_messages("log.handleQtMessages");

    // message pattern
    QString value = rProperties.property(key_message_pattern);
    if (!value.isNull())
    {
        LogManager::setMessagePattern(value);
       // logger()->debug("Set messagePattern to %1", QVariant(LogManager::handleQtMessages()).toString());
    }

    // Handle Qt messages
    value = rProperties.property(key_handle_qt_messages);
    if (!value.isNull())
    {
        LogManager::setHandleQtMessages(OptionConverter::toBoolean(value, false));
       // logger()->debug("Set handling of Qt messages LoggerRepository to %1", QVariant(LogManager::handleQtMessages()).toString());
    }
}
void PropertyConfigurator::configureRootLogger(const Properties &rProperties,
                                                   LoggerRepository *pLoggerRepository)
{
    Q_ASSERT_X(pLoggerRepository, "PropertyConfigurator::configureRootLogger()", "pLoggerRepository must not be null.");

    const QLatin1String key_root_logger("log.rootLogger");
    const QLatin1String key_root_category("log.rootCategory");

    QString key = key_root_logger;
    QString value = OptionConverter::findAndSubst(rProperties, key);

    if (value.isNull()) {
        key = key_root_category;
        value = OptionConverter::findAndSubst(rProperties, key);
    }

    if (value.isNull())
        logger()->write("Could not find root logger information. Is this correct?");
    else
        parseLogger(rProperties, pLoggerRepository->rootLogger(), key, value);
}

void PropertyConfigurator::configureNonRootElements(const Properties &rProperties,
                                                    LoggerRepository *pLoggerRepository)
{
    Q_ASSERT_X(pLoggerRepository, "PropertyConfigurator::configureNonRootElements()", "pLoggerRepository must not be null.");

    const QString logger_prefix = QLatin1String("log.logger.");
    const QString category_prefix = QLatin1String("log.category.");

    QStringList keys = rProperties.propertyNames();
    QString key;
    Q_FOREACH(key, keys)
    {
        QString java_name;
        if (key.startsWith(logger_prefix))
            java_name = key.mid(logger_prefix.length());
        else if (key.startsWith(category_prefix))
            java_name = key.mid(category_prefix.length());
        QString cpp_name = OptionConverter::classNameJavaToCpp(java_name);
        if (!java_name.isEmpty())
        {
            Logger *p_logger = pLoggerRepository->logger(cpp_name);
            QString value = OptionConverter::findAndSubst(rProperties, key);
            parseLogger(rProperties, p_logger, key, value);
        }
    }
}

void PropertyConfigurator::parseLogger(const Properties &rProperties,
                                           Logger *pLogger,
                                           const QString &rKey,
                                           const QString &rValue)
{
    Q_ASSERT_X(pLogger, "PropertyConfigurator::parseLogger()", "pLogger must not be null.");
    Q_ASSERT_X(!rKey.isEmpty(), "PropertyConfigurator::parseLogger()", "rKey must not be empty.");

    QStringList appenders = rValue.split(QLatin1Char(','));

    QStringListIterator i (appenders);
    QString value;

    pLogger->removeAllAppenders();
    while(i.hasNext())
    {
        value = i.next().trimmed();
        if(value.isEmpty())
            continue;

        Appender *p_appender = parseAppender(rProperties, value);
        if (p_appender)
            pLogger->addAppender(p_appender);
    }
}


Appender *PropertyConfigurator::parseAppender(const Properties &rProperties, const QString &rName)
{
    const QLatin1String appender_prefix("log.appender.");

    if (m_hashAppenderRegistry.contains(rName)) {
        return m_hashAppenderRegistry.value(rName);
    }

    QString key = appender_prefix + rName;
    QString value = OptionConverter::findAndSubst(rProperties, key);
    if (value.isNull()) {
        logger()->write(QString("Missing appender definition for appender named '%1'").arg(rName));
        return 0;
    }

    Appender *p_appender = Factory::createAppender(value);
    if (!p_appender) {
        logger()->write(QString("Unable to create appender of class '%1' namd '%2'").arg(value).arg(rName));
        return 0;
    }

    p_appender->setName(rName);

    QStringList exclusions;
    exclusions << QLatin1String("layout");
    setProperties(rProperties, key + QLatin1String("."), exclusions, p_appender);

    p_appender->open();

    m_hashAppenderRegistry.insert(rName, p_appender);
    return p_appender;
}

void PropertyConfigurator::setProperties(const Properties &rProperties,
                                             const QString &rPrefix,
                                             const QStringList &rExclusions,
                                             QObject *pObject)
{
    Q_ASSERT_X(!rPrefix.isEmpty(), "PropertyConfigurator::setProperties()", "rPrefix must not be empty.");
    Q_ASSERT_X(pObject, "PropertyConfigurator::setProperties()", "pObject must not be null.");

    QStringList keys = rProperties.propertyNames();
    QString key;
    Q_FOREACH(key, keys)
    {
        if (!key.startsWith(rPrefix))
            continue;
        QString property = key.mid(rPrefix.length());
        if (property.isEmpty())
            continue;
        QStringList split_property = property.split(QLatin1Char('.'));
        if (rExclusions.contains(split_property.at(0), Qt::CaseInsensitive))
            continue;
        QString value = OptionConverter::findAndSubst(rProperties, key);
        Factory::setObjectProperty(pObject, property, value);
    }
}

void PropertyConfigurator::startCaptureErrors()
{
   Q_ASSERT_X(!m_pConfigureErrors, "PropertyConfigurator::startCaptureErrors()", "m_pConfigureErrors must be empty.");

   m_pConfigureErrors = new ListAppender(0);
   m_pConfigureErrors->setName(QLatin1String("PropertyConfigurator"));
   m_pConfigureErrors->setConfiguratorList(true);
   //LogManager::logLogger()->addAppender(m_pConfigureErrors);
   logger()->addAppender(m_pConfigureErrors);
}


bool PropertyConfigurator::stopCaptureErrors()
{
   Q_ASSERT_X(m_pConfigureErrors, "PropertyConfigurator::stopCaptureErrors()", "m_pConfigureErrors must not be empty.");

   //LogManager::logLogger()->removeAppender(m_pConfigureErrors);
    logger()->removeAppender(m_pConfigureErrors);
   //ConfiguratorHelper::setConfigureError(m_pConfigureErrors->list());
   bool result = (m_pConfigureErrors->list().count() == 0);
   delete m_pConfigureErrors;
   m_pConfigureErrors = 0;
   return result;
}
