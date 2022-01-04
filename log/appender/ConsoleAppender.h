#ifndef CONSOLEAPPENDER_H
#define CONSOLEAPPENDER_H

#include "WriterAppender.h"

class ConsoleAppender :  public WriterAppender
{
    Q_OBJECT

    Q_PROPERTY(QString target READ target WRITE setTarget)

public:
    enum Target {
        /*! The output target is standard out. */
        STDOUT_TARGET,
        /*! The output target is standard error. */
        STDERR_TARGET
    };
    Q_ENUMS(Target)

    explicit ConsoleAppender(QObject *parent = NULL);
    explicit ConsoleAppender(const QString &target, QObject *parent = NULL);
    explicit ConsoleAppender(Target target, QObject *parent = NULL);
    ~ConsoleAppender();

private:
    ConsoleAppender(const ConsoleAppender &rOther); // Not implemented
    ConsoleAppender &operator=(const ConsoleAppender &rOther); // Not implemented

public:
    QString target() const;
    void setTarget(const QString &rTarget);
    void setTarget(Target target);

    virtual void open() ;
    virtual void close();

protected:
    void closeStream();

private:
    volatile Target  m_target;
    QTextStream *m_pTextStream;
};

inline void ConsoleAppender::setTarget(Target target)
 {  m_target = target;   }

#endif // CONSOLEAPPENDER_H
