#include "ConsoleAppender.h"
#include <QTextStream>
#include "../helpers/OptionConverter.h"

ConsoleAppender::ConsoleAppender(QObject *parent) :
    WriterAppender(parent),
    m_target(STDOUT_TARGET),
    m_pTextStream(0)
{
}

ConsoleAppender:: ConsoleAppender(const QString &target,QObject *parent) :
    WriterAppender(parent),
    m_pTextStream(0)
{
    setTarget(target);
}

ConsoleAppender:: ConsoleAppender(Target target,QObject *parent) :
    WriterAppender(parent),
    m_target(target),
    m_pTextStream(0)
{
}

ConsoleAppender::~ConsoleAppender()
{
    close();
}

QString ConsoleAppender::target() const
{
    if (m_target == STDOUT_TARGET)
        return QLatin1String("STDOUT_TARGET");
    else
        return QLatin1String("STDERR_TARGET");
}

void ConsoleAppender::setTarget(const QString &rTarget)
{
    bool ok;
    Target target = (Target)OptionConverter::toTarget(rTarget, &ok);
    if (ok)
        setTarget(target);
}


void ConsoleAppender::open()
{
    closeStream();

    if (m_target == STDOUT_TARGET)
        m_pTextStream = new QTextStream(stdout);
    else
        m_pTextStream = new QTextStream(stderr);
    setWriter(m_pTextStream);

    WriterAppender::open();
}
void ConsoleAppender::close()
{
    WriterAppender::close();
    closeStream();
}

void ConsoleAppender::closeStream()
{
    if(!m_pTextStream)
        return;

    setWriter(0);
    delete m_pTextStream;
    m_pTextStream = 0;
}
