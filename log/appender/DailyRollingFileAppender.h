#ifndef DAILYROLLINGFILEAPPENDER_H
#define DAILYROLLINGFILEAPPENDER_H

#include <QDateTime>
#include "FileAppender.h"

class DailyRollingFileAppender : public FileAppender
{
    Q_OBJECT

    Q_PROPERTY(QString datePattern READ datePattern WRITE setDatePattern)

public:
    enum DatePattern
    {
        /*! The minutely date pattern string is "'.'yyyy-MM-dd-hh-mm". */
        MINUTELY_ROLLOVER = 0,
        /*! The hourly date pattern string is "'.'yyyy-MM-dd-hh". */
        HOURLY_ROLLOVER,
        /*! The half-daily date pattern string is "'.'yyyy-MM-dd-a". */
        HALFDAILY_ROLLOVER,
        /*! The daily date pattern string is "'.'yyyy-MM-dd". */
        DAILY_ROLLOVER,
        /*! The weekly date pattern string is "'.'yyyy-ww". */
        WEEKLY_ROLLOVER,
        /*! The monthly date pattern string is "'.'yyyy-MM". */
        MONTHLY_ROLLOVER
    };
    Q_ENUMS(DatePattern)

    explicit DailyRollingFileAppender(QObject *parent = NULL);
    explicit DailyRollingFileAppender(const QString &fileName, QObject *parent = NULL);
    explicit DailyRollingFileAppender(const QString &fileName, const QString &datePattern,  QObject *parent = NULL);
    ~DailyRollingFileAppender();

private:
    DailyRollingFileAppender(const DailyRollingFileAppender &rOther); // Not implemented
    DailyRollingFileAppender &operator=(const DailyRollingFileAppender &rOther); // Not implemented

public:
    QString datePattern() const;
    void setDatePattern(const QString &datePattern);

    void setDatePattern(DatePattern datePattern);

    virtual void open();
    virtual void write(const QString &message);

protected:
    void computeFrequency();
    void computeRollOverTime();
    QString frequencyToString() const;
    void rollOver();

private:
    QString m_datePattern;
    DatePattern m_frequency;
    QString m_activeDatePattern;
    QDateTime m_rollOverTime;
    QString m_rollOverSuffix;
};

inline QString DailyRollingFileAppender::datePattern() const
{   return m_datePattern;   }

inline void DailyRollingFileAppender::setDatePattern(const QString &datePattern)
{   m_datePattern = datePattern;    }


#endif // DAILYROLLINGFILEAPPENDER_H
