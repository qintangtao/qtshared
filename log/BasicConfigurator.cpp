#include "BasicConfigurator.h"
#include "LogManager.h"
#include "Logger.h"
#include "appender/ConsoleAppender.h"

bool BasicConfigurator::configure()
{
#if 0
    LogObjectPtr<ListAppender> list = new ListAppender;
    list->setName(QLatin1String("BasicConfigurator"));
    list->setConfiguratorList(true);
    list->setThreshold(Level::ERROR_INT);
    LogManager::logLogger()->addAppender(list);

    PatternLayout *p_layout = new PatternLayout(PatternLayout::TTCC_CONVERSION_PATTERN);
    p_layout->setName(QLatin1String("BasicConfigurator TTCC"));
    p_layout->activateOptions();
    ConsoleAppender *p_appender = new ConsoleAppender(p_layout, ConsoleAppender::STDOUT_TARGET);
    p_appender->setName(QLatin1String("BasicConfigurator stdout"));
    p_appender->activateOptions();
    LogManager::rootLogger()->addAppender(p_appender);

    LogManager::logLogger()->removeAppender(list);
    ConfiguratorHelper::setConfigureError(list->list());
    return (list->list().count() == 0);
#endif

    ConsoleAppender *p_appender = new ConsoleAppender(ConsoleAppender::STDOUT_TARGET);
    p_appender->setName(QLatin1String("BasicConfigurator stdout"));
    p_appender->open();
    LogManager::rootLogger()->addAppender(p_appender);
    return true;
}

void BasicConfigurator::configure(Appender *pAppender)
{
    LogManager::rootLogger()->addAppender(pAppender);
}

void BasicConfigurator::resetConfiguration()
{
   LogManager::resetConfiguration();
}
