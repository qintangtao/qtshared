#ifndef DATETIME_H
#define DATETIME_H
#include <QDateTime>

class DateTime : public QDateTime
{
public:
    DateTime();

    // DateTime(const DateTime &rOther); // Use compiler default

    DateTime(const QDateTime &rOther);

    DateTime(const QDate &rDate,
             const QTime &rTime,
             Qt::TimeSpec timeSpec = Qt::LocalTime);

    // virtual ~DateTime(); // Use compiler default

    DateTime &operator=(const DateTime &rOther);

    qint64 toMilliSeconds() const;

    QString toString(const QString &rFormat) const;

    static DateTime currentDateTime();

    static DateTime fromMilliSeconds(qint64 milliSeconds);

private:
    QString formatDateTime(const QString &rFormat) const;
    QString formatToken(const QString &rToken, bool am_pm) const;
    static bool hasAMPM(const QString &rFormat);
};


inline DateTime::DateTime() : QDateTime()
{}

inline DateTime::DateTime(const QDateTime &rOther) : QDateTime(rOther)
{}

inline DateTime::DateTime(const QDate &rDate,
                          const QTime &rTime,
                          Qt::TimeSpec timeSpec) :
    QDateTime(rDate, rTime, timeSpec)
{}

inline DateTime &DateTime::operator=(const DateTime &rOther)
{   QDateTime::operator=(rOther); return *this; }

inline qint64 DateTime::toMilliSeconds() const
{   return (qint64)1000 * toTime_t() + time().msec();   }

inline DateTime DateTime::currentDateTime()
{   return DateTime(QDateTime::currentDateTime());    }

inline DateTime DateTime::fromMilliSeconds(qint64 milliSeconds)
{   return DateTime(QDateTime::fromTime_t(milliSeconds / 1000).addMSecs(milliSeconds % 1000));    }

#endif // DATETIME_H
