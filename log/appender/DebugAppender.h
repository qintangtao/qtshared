#ifndef DEBUGAPPENDER_H
#define DEBUGAPPENDER_H

#include "WriterAppender.h"

class DebugAppender :  public WriterAppender
{
    Q_OBJECT

public:
    explicit DebugAppender(QObject *parent = NULL);
    ~DebugAppender();

private:
    DebugAppender(const DebugAppender &rOther); // Not implemented
    DebugAppender &operator=(const DebugAppender &rOther); // Not implemented

public:
    virtual void open();
    virtual void close() ;
     virtual void write(const QString &message);
};


#endif // DEBUGAPPENDER_H
