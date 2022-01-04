#ifndef ROLLINGFILEAPPENDER_H
#define ROLLINGFILEAPPENDER_H

#include "FileAppender.h"

class RollingFileAppender : public FileAppender
{
    Q_OBJECT

    Q_PROPERTY(int maxBackupIndex READ maxBackupIndex WRITE setMaxBackupIndex)
    Q_PROPERTY(qint64 maximumFileSize READ maximumFileSize WRITE setMaximumFileSize)
    Q_PROPERTY(QString maxFileSize WRITE setMaxFileSize)

public:
    explicit RollingFileAppender(QObject *parent = NULL);
    explicit RollingFileAppender(const QString &fileName, QObject *parent = NULL);
    explicit RollingFileAppender(const QString &fileName,
                                bool append,
                                QObject *parent = 0);
    ~RollingFileAppender();

private:
    RollingFileAppender(const RollingFileAppender &rOther); // Not implemented
    RollingFileAppender &operator=(const RollingFileAppender &rOther); // Not implemented

public:
    int maxBackupIndex() const;
    void setMaxBackupIndex(int maximumFileSize);

    qint64 maximumFileSize() const;
    void setMaximumFileSize(qint64 maximumFileSize);

    void setMaxFileSize(const QString &rMaxFileSize);

    virtual void write(const QString &message);

protected:
    void rollOver();

private:
    int m_nMaxBackupIndex;
    qint64 m_nMaximumFileSize;
};

inline int RollingFileAppender::maxBackupIndex() const
{ return m_nMaxBackupIndex; }
inline void RollingFileAppender::setMaxBackupIndex(int maxBackupIndex)
{ m_nMaxBackupIndex = maxBackupIndex; }

inline qint64 RollingFileAppender::maximumFileSize() const
{ return m_nMaximumFileSize; }
inline void RollingFileAppender::setMaximumFileSize(qint64 maximumFileSize)
{ m_nMaximumFileSize = maximumFileSize; }

#endif // ROLLINGFILEAPPENDER_H
