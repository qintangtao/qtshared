#include "DebugAppender.h"
#ifdef Q_OS_WIN32
#include <windows.h>
#endif

DebugAppender::DebugAppender(QObject *parent) :
    WriterAppender(parent)
{
}

DebugAppender::~DebugAppender()
{

}


void DebugAppender::open()
{
    WriterAppender::open();
    writeHeader();
}
void DebugAppender::close()
{
    writeFooter();
    WriterAppender::close();
}
void DebugAppender::write(const QString &message)
{
#if defined(Q_OS_WIN32)
        OutputDebugStringW(reinterpret_cast<const WCHAR*>(message.utf16()));
#else
        fprintf(stderr, message.toLocal8Bit().data());
        fflush(stderr);
#endif
}
