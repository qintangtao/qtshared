#include "FileAppender.h"
#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QDir>
#include <QTextStream>

FileAppender::FileAppender(QObject *parent) :
    WriterAppender(parent),
    m_bAppendFile(false),
    m_bBufferedIo(true),
    m_pFile(0),
    m_pTextStream(0)
{
}
FileAppender::FileAppender(const QString &fileName, QObject *parent) :
    WriterAppender(parent),
    m_bAppendFile(false),
    m_bBufferedIo(true),
    m_fileName(fileName),
    m_pFile(0),
    m_pTextStream(0)
{

}
FileAppender::FileAppender(const QString &fileName,
                     bool append,
                     QObject *parent):
    WriterAppender(parent),
    m_bAppendFile(append),
    m_bBufferedIo(true),
    m_fileName(fileName),
    m_pFile(0),
    m_pTextStream(0)
{

}

FileAppender::FileAppender(const QString &fileName,
                     bool append,
                     bool buffered,
                     QObject *parent):
    WriterAppender(parent),
    m_bAppendFile(append),
    m_bBufferedIo(buffered),
    m_fileName(fileName),
    m_pFile(0),
    m_pTextStream(0)
{

}

FileAppender::~FileAppender()
{
    close();
}

void FileAppender::open()
{
    if (m_fileName.isEmpty())
        return;

    closeFile();
    openFile();
    WriterAppender::open();
}

void FileAppender::close()
{
    WriterAppender::close();
    closeFile();
}

void FileAppender::openFile()
{
    if (m_fileName.isEmpty())
        return;

    QFileInfo file_info(m_fileName);
    QDir parent_dir = file_info.dir();
    if (!parent_dir.exists())
    {
        QString name = parent_dir.dirName();
        parent_dir.cdUp();
        parent_dir.mkdir(name);
    }

    m_pFile = new QFile(m_fileName);
    QFile::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
    if (m_bAppendFile)
        mode |= QIODevice::Append;
    else
        mode |= QIODevice::Truncate;
    if (!m_bBufferedIo)
        mode |= QIODevice::Unbuffered;
    if (!m_pFile->open(mode))
    {
        return;
    }

    m_pTextStream = new QTextStream(m_pFile);
    setWriter(m_pTextStream);
}
void FileAppender::closeFile()
{
    setWriter(0);
    if (m_pTextStream) {
        delete m_pTextStream;
        m_pTextStream = 0;
    }
    if(m_pFile) {
        delete m_pFile;
        m_pFile = 0;
    }
}

bool FileAppender::removeFile(QFile &rFile) const
{
    if (rFile.remove())
        return true;

    return false;
}


bool FileAppender::renameFile(QFile &rFile,
                              const QString &rFileName) const
{
    if (rFile.rename(rFileName))
        return true;
    return false;
}

#if 0
bool FileAppender::open()
{
    QString currentDate = QDate::currentDate().toString("yyyyMMdd");
    if(m_f && (m_f->size() > m_log->maxFileSize() || m_oldDate != currentDate)) {
        close();
    }

    if (!m_f) {
        m_oldDate = currentDate;

        m_f = new QFile(createNewFileName(currentDate));
        if (!m_f->open(QIODevice::WriteOnly | QIODevice::Append)) {
            delete m_f;
            m_f = 0;
            return false;
        }

        m_f->write("\n");
        m_f->write(QTime::currentTime().toString("HH:mm:ss.zzz").toLatin1());
        m_f->write(" --- DEBUG LOG OPENED ---\n");
        m_f->flush();
    }

    return true;
}
void FileAppender::close()
{
    if (!m_f)
        return;
    m_f->write(QTime::currentTime().toString("HH:mm:ss.zzz").toLatin1());
    m_f->write(" --- DEBUG LOG CLOSED ---\n\n");
    m_f->flush();
    m_f->close();
    delete m_f;
    m_f = 0;
}
void FileAppender::write(const QString &byte)
{
    if (!m_f)
        return;
    m_f->write(byte.toLocal8Bit());
    m_f->flush();
}

QString FileAppender::createNewFileName(const QString &date)
{
    QString logPath;

    QDir dir(m_log->path());

    for (int i  = 1; i < INT_MAX; i++) {
        logPath = dir.absoluteFilePath(QString("%1%2.%3")
                                       .arg(date)
                                       .arg(i == 1 ? "" : QString("-%1").arg(i))
                                       .arg(m_log->fileSuffix()));

        QFileInfo fi(logPath);
        if (!fi.exists() || fi.size() < m_log->maxFileSize())
            break;
    }

    return logPath;
}

#endif
