#include "WriterAppender.h"
#include <QTextStream>

#define DEFAULT_HEADER      "===============================START===============================\n"
#define DEFAULT_FOOTER      "================================END================================\n"

WriterAppender::WriterAppender(QObject *parent) :
    Appender(parent) ,
    m_pStream(NULL),
    m_header(DEFAULT_HEADER),
    m_footer(DEFAULT_FOOTER)
{
}

WriterAppender::WriterAppender(QTextStream *stream, QObject *parent)  :
    Appender(parent) ,
    m_pStream(stream),
    m_header(DEFAULT_HEADER),
    m_footer(DEFAULT_FOOTER)
{
}

WriterAppender::~WriterAppender()
{
    close();
}

void WriterAppender::close()
{
    closeWriter();
}

void WriterAppender::write(const QString &message)
{
    if (NULL == m_pStream)
        return;

    *m_pStream << message;
    m_pStream->flush();
}

void WriterAppender::setWriter(QTextStream *pStream)
{
    closeWriter();
    m_pStream = pStream;
    writeHeader();
}

void WriterAppender::closeWriter()
{
    if (!m_pStream)
        return;

    writeFooter();
    m_pStream = 0;
}

void WriterAppender::writeFooter() const
{
    if (!m_pStream)
        return;

    *m_pStream << m_footer;
}

void WriterAppender::writeHeader() const
{
    if (!m_pStream)
        return;

    *m_pStream << m_header;
}
