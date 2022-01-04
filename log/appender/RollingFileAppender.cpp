#include "RollingFileAppender.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include "../helpers/OptionConverter.h"

#define DEFAULT_MAX_BACKUP_INDEX      1
#define DEFAULT_MAXIMUM_FILESIZE        4*1024*1024

RollingFileAppender::RollingFileAppender(QObject *parent) :
    FileAppender(parent),
    m_nMaxBackupIndex(DEFAULT_MAX_BACKUP_INDEX),
    m_nMaximumFileSize(DEFAULT_MAXIMUM_FILESIZE)
{
}
RollingFileAppender::RollingFileAppender(const QString &fileName, QObject *parent) :
    FileAppender(fileName, parent),
    m_nMaxBackupIndex(DEFAULT_MAX_BACKUP_INDEX),
    m_nMaximumFileSize(DEFAULT_MAXIMUM_FILESIZE)
{
}
RollingFileAppender::RollingFileAppender(const QString &fileName,
                            bool append,
                            QObject *parent) :
    FileAppender(fileName, append, parent),
    m_nMaxBackupIndex(DEFAULT_MAX_BACKUP_INDEX),
    m_nMaximumFileSize(DEFAULT_MAXIMUM_FILESIZE)
{
}
RollingFileAppender::~RollingFileAppender()
{
    close();
}

void RollingFileAppender::setMaxFileSize(const QString &rMaxFileSize)
{
    bool ok;
    qint64 max_file_size = OptionConverter::toFileSize(rMaxFileSize, &ok);
    if (ok)
        setMaximumFileSize(max_file_size);
}

void RollingFileAppender::write(const QString &message)
{
    FileAppender::write(message);
    if (writer()->device()->size() > this->m_nMaximumFileSize)
        rollOver();
}

void RollingFileAppender::rollOver()
{
    closeFile();

    QFile f;
    f.setFileName(file() + QLatin1Char('.') + QString::number(m_nMaxBackupIndex));
    if (f.exists() && !removeFile(f))
        return;

    QString target_file_name;
    int i;
    for (i = m_nMaxBackupIndex - 1; i >=1; i--)
    {
        f.setFileName(file() + QLatin1Char('.') + QString::number(i));
        if (f.exists())
        {
            target_file_name = file() + QLatin1Char('.') + QString::number(i + 1);
            if (!renameFile(f, target_file_name))
                return;
        }
    }

    f.setFileName(file());
    target_file_name = file() + QLatin1String(".1");
    if (!renameFile(f, target_file_name))
        return;

    openFile();
}
