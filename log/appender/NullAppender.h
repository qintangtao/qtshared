#ifndef NullAppender_H
#define NullAppender_H

#include "WriterAppender.h"

class NullAppender :  public WriterAppender
{
    Q_OBJECT

public:
    explicit NullAppender(QObject *parent = NULL);
    ~NullAppender();

private:
    NullAppender(const NullAppender &rOther); // Not implemented
    NullAppender &operator=(const NullAppender &rOther); // Not implemented

public:
     virtual void write(const QString &message);
};


#endif // NullAppender_H
