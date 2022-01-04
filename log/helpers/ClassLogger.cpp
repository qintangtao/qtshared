#include "ClassLogger.h"
#include "../LogManager.h"

ClassLogger::ClassLogger() :
        mpLogger(0)
{
}


Logger *ClassLogger::logger(const QObject *pObject)
{
    Q_ASSERT_X(pObject, "ClassLogger::logger()", "pObject must not be null");
#if QT_VERSION < QT_VERSION_CHECK(4, 4, 0)
    if (!mpLogger)
        q_atomic_test_and_set_ptr(&mpLogger,
                                  0,
                                  LogManager::logger(QLatin1String(pObject->metaObject()->className())));
    return const_cast<Logger *>(mpLogger);
#else
    if (!static_cast<Logger *>(mpLogger))
         mpLogger.testAndSetOrdered(0,
                                    LogManager::logger(QLatin1String(pObject->metaObject()->className())));
    return const_cast<Logger *>(static_cast<Logger *>(mpLogger));
#endif
}
