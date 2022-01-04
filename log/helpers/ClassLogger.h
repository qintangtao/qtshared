#ifndef CLASSLOGGER_H
#define CLASSLOGGER_H

#include <QtCore/QObject>
#if QT_VERSION >= QT_VERSION_CHECK(4, 4, 0)
#	include <QtCore/QAtomicPointer>
#	ifndef Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
#		warning "QAtomicPointer test and set is not native. The class Log4Qt::ClassLogger is not thread-safe."
#	endif
#endif

class Logger;

class ClassLogger
{
public:
    /*!
     * Creates a ClassLogger object.
     */
    ClassLogger();
    // ~ClassLogger(); // Use compiler default
    // ClassLogger(const ClassLogger &rOther); // Use compiler default
    // ClassLogger &operator=(const ClassLogger &rOther); // Use compiler default

    Logger *logger(const QObject *pObject);

private:
#if QT_VERSION < QT_VERSION_CHECK(4, 4, 0)
    volatile Logger *mpLogger;
#else
    mutable QAtomicPointer<Logger> mpLogger;
#endif
};

#endif // CLASSLOGGER_H
