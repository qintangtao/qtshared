#ifndef PROPERTYCONFIGURATOR_H
#define PROPERTYCONFIGURATOR_H

#include <QString>
#include <QSettings>
#include "helpers/Properties.h"

class Appender;
class ListAppender;
class Logger;
class LoggerRepository;

class PropertyConfigurator
{
public:
    PropertyConfigurator();
    // virtual ~PropertyConfigurator(); // Use compiler default
private:
    PropertyConfigurator(const PropertyConfigurator &rOther); // Not implemented
    PropertyConfigurator &operator=(const PropertyConfigurator &rOther); // Not implemented

public:
    static bool configure(const Properties &rProperties);
    static bool configure(const QString &rConfigFilename);
    static bool configure(const QSettings &rSettings);

    bool doConfigure(const Properties &rProperties,
                     LoggerRepository *pLoggerRepository = 0);
    bool doConfigure(const QString &rConfigFileName,
                     LoggerRepository *pLoggerRepository = 0);
    bool doConfigure(const QSettings &rSettings,
                     LoggerRepository *pLoggerRepository = 0);

private:
    void configureFromProperties(const Properties &rProperties,
                                 LoggerRepository *pLoggerRepository);
    void configureFromFile(const QString &rConfigFileName,
                           LoggerRepository *pLoggerRepository);
    void configureFromSettings(const QSettings &rSettings,
                               LoggerRepository *pLoggerRepository);
    void configureGlobalSettings(const Properties &rProperties,
                                         LoggerRepository *pLoggerRepository) const;
    void configureRootLogger(const Properties &rProperties,
                                     LoggerRepository *pLoggerRepository);
    void configureNonRootElements(const Properties &rProperties,
                                  LoggerRepository *pLoggerRepository);
    void parseLogger(const Properties &rProperties,
                             Logger *pLogger,
                             const QString &rKey,
                             const QString &rValue);
    Appender *parseAppender(const Properties &rProperties,
                                                 const QString &rName);
    void setProperties(const Properties &rProperties,
                               const QString &rPrefix,
                               const QStringList &rExclusions,
                               QObject *pObject);
    void startCaptureErrors();
    bool stopCaptureErrors();

private:
    QHash<QString, Appender*> m_hashAppenderRegistry;
    ListAppender *m_pConfigureErrors;
};

inline PropertyConfigurator::PropertyConfigurator() : m_pConfigureErrors(0)
{}

#endif // PROPERTYCONFIGURATOR_H
