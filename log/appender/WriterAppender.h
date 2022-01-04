#ifndef WriterAppender_H
#define WriterAppender_H

#include "Appender.h"

class QTextStream;

class WriterAppender :  public Appender
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString header READ header WRITE setHeader)
    Q_PROPERTY(QString footer READ footer WRITE setFooter)

public:
    explicit WriterAppender(QObject *parent = NULL);
    explicit WriterAppender(QTextStream *stream, QObject *parent = 0);
    ~WriterAppender();

private:
    WriterAppender(const WriterAppender &rOther); // Not implemented
    WriterAppender &operator=(const WriterAppender &rOther); // Not implemented

public:
    QString header() const;
    void setHeader(const QString &header);
    QString footer() const;
    void setFooter(const QString &footer);

    virtual QString name() const;
    virtual void setName(const QString &rName);
    virtual void open() {}
    virtual void close() ;
    virtual void write(const QString &message);

protected:
    QTextStream *writer() const;
    void setWriter(QTextStream *pStream);
    void closeWriter();
    void writeFooter() const;
    void writeHeader() const;

private:
    QTextStream *m_pStream;
    QString m_header;
    QString m_footer;
};


inline QString WriterAppender::header() const
{	 return m_header;	}
inline void WriterAppender::setHeader(const QString &header)
{ m_header = header; }
inline QString WriterAppender::footer() const
{	 return m_footer;	}
inline void WriterAppender::setFooter(const QString &footer)
{ m_footer = footer; }

inline QString WriterAppender::name() const
{	 return objectName();	}

inline void WriterAppender::setName(const QString &rName)
{   setObjectName(rName);  }

inline QTextStream *WriterAppender::writer() const
{ return m_pStream; }

#endif // WriterAppender_H
