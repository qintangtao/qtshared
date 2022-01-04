#ifndef FILEAPPENDER_H
#define FILEAPPENDER_H

#include "WriterAppender.h"


class QFile;

class FileAppender : public WriterAppender
{
    Q_OBJECT

    Q_PROPERTY(bool appendFile READ appendFile WRITE setAppendFile)
    Q_PROPERTY(bool bufferedIo READ bufferedIo WRITE setBufferedIo)
    Q_PROPERTY(QString file READ file WRITE setFile)

public:
    explicit FileAppender(QObject *parent = NULL);
    explicit FileAppender(const QString &fileName, QObject *parent = NULL);
    explicit FileAppender(const QString &fileName,
                         bool append,
                         QObject *parent = 0);
    explicit FileAppender(const QString &fileName,
                         bool append,
                         bool buffered,
                         QObject *parent = 0);
    ~FileAppender();

private:
    FileAppender(const FileAppender &rOther); // Not implemented
    FileAppender &operator=(const FileAppender &rOther); // Not implemented

public:
    bool appendFile() const;
    void setAppendFile(bool append);
    bool bufferedIo() const;
    void setBufferedIo(bool buffered);
    QString file() const;
    void setFile(const QString &rFileName);

    virtual void open();
    virtual void close();

protected:
    void openFile();
    void closeFile();
    bool removeFile(QFile &rFile) const;
    bool renameFile(QFile &rFile,
                    const QString &rFileName) const;

private:
    QString createNewFileName(const QString &date) { return "";}

private:
    bool m_bAppendFile;
    bool m_bBufferedIo;
    QString m_fileName;
    QFile* m_pFile;
    QTextStream *m_pTextStream;
    QString m_oldDate;
};

inline bool FileAppender::appendFile() const
{ return m_bAppendFile; }
inline void FileAppender::setAppendFile(bool append)
{ m_bAppendFile = append; }

inline bool FileAppender::bufferedIo() const
{ return m_bBufferedIo; }
inline void FileAppender::setBufferedIo(bool buffered)
{ m_bBufferedIo = buffered; }

inline QString FileAppender::file() const
{   return m_fileName;   }
inline void FileAppender::setFile(const QString &fileName)
{   m_fileName = fileName;   }

#endif // FILEAPPENDER_H
