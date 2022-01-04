#ifndef LOGDEF_H
#define LOGDEF_H

#include <QtCore/QAtomicPointer>
#include <QtCore/QHash>

#define LOG_VERSION 0x000200

/*!
 * The macro expands to a string that specifies the Log4Qt's version
 * number. This is the version against which the application is compiled.
 *
 * \sa \ref Log4Qt::LOG4QT_VERSION "LOG4QT_VERSION",
 *     \ref Log4Qt::LogManager::version() "LogManager::version()"
 */
#define LOG_VERSION_STR "0.1.0"

#if QT_VERSION < QT_VERSION_CHECK(4, 4, 0)
    #define LOG_GLOBAL_STATIC(TYPE, FUNCTION)                              \
        static volatile TYPE *sp_global_static_##FUNCTION = 0;                \
        TYPE *FUNCTION()                                                      \
        {                                                                     \
            if (!sp_global_static_##FUNCTION)                                 \
            {                                                                 \
                TYPE *p_temp = new TYPE;                                      \
                if (!q_atomic_test_and_set_ptr(&sp_global_static_##FUNCTION,  \
                                               0, p_temp))                    \
                    delete p_temp;                                            \
            }                                                                 \
            return const_cast<TYPE *>(sp_global_static_##FUNCTION);           \
        }
#else
    #define LOG_GLOBAL_STATIC(TYPE, FUNCTION)                              \
        static QBasicAtomicPointer<TYPE > sp_global_static_##FUNCTION =       \
            Q_BASIC_ATOMIC_INITIALIZER(0);                                    \
        TYPE *FUNCTION()                                                      \
        {                                                                     \
            if (!sp_global_static_##FUNCTION)                                 \
            {                                                                 \
                TYPE *p_temp = new TYPE;                                      \
                if (!sp_global_static_##FUNCTION.testAndSetOrdered(0,         \
                                                                   p_temp))   \
                    delete p_temp;                                            \
            }                                                                 \
            return sp_global_static_##FUNCTION;                               \
        }
#endif

#if QT_VERSION < QT_VERSION_CHECK(4, 4, 0)
    #define LOG_IMPLEMENT_INSTANCE(TYPE)                                   \
        static TYPE *sp_singleton_##TYPE = 0;                                 \
        TYPE *TYPE::instance()                                                \
        {                                                                     \
            if (!sp_singleton_##TYPE)                                         \
            {                                                                 \
                TYPE *p_temp = new TYPE;                                      \
                if (!q_atomic_test_and_set_ptr(&sp_singleton_##TYPE,          \
                                               0, p_temp))                    \
                     delete p_temp;                                           \
            }                                                                 \
            return sp_singleton_##TYPE;                                       \
        }
#else
    #define LOG_IMPLEMENT_INSTANCE(TYPE)                                   \
        static QBasicAtomicPointer<TYPE > sp_singleton_##TYPE =               \
            Q_BASIC_ATOMIC_INITIALIZER(0);                                    \
        TYPE *TYPE::instance()                                                \
        {                                                                     \
            if (!sp_singleton_##TYPE)                                         \
            {                                                                 \
                TYPE *p_temp = new TYPE;                                      \
                if (!sp_singleton_##TYPE.testAndSetOrdered(0, p_temp))        \
                    delete p_temp;                                            \
            }                                                                 \
            return sp_singleton_##TYPE;                                       \
        }
#endif


#endif // LOGDEF_H
