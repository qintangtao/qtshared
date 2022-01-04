#include "NullAppender.h"

NullAppender::NullAppender(QObject *parent) :
    WriterAppender(parent)
{
}

NullAppender::~NullAppender()
{

}

void NullAppender::write(const QString &message)
{
    Q_UNUSED(message);
}
