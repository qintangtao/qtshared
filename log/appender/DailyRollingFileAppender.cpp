#include "DailyRollingFileAppender.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QMetaEnum>
#include "../helpers/DateTime.h"

#define DEFAULT_MAX_BACKUP_INDEX      10
#define DEFAULT_MAXIMUM_FILESIZE        1024

DailyRollingFileAppender::DailyRollingFileAppender(QObject *parent) :
    FileAppender(parent)
{
}
DailyRollingFileAppender::DailyRollingFileAppender(const QString &fileName, QObject *parent) :
    FileAppender(fileName, parent)
{

}
DailyRollingFileAppender::DailyRollingFileAppender(const QString &fileName, const QString &datePattern,  QObject *parent) :
    FileAppender(fileName, parent)
{
    setDatePattern(datePattern);
}
DailyRollingFileAppender::~DailyRollingFileAppender()
{
    close();
}

void DailyRollingFileAppender::setDatePattern(DatePattern datePattern)
{
    switch (datePattern)
    {
        case MINUTELY_ROLLOVER:
            setDatePattern(QLatin1String("'.'yyyy-MM-dd-hh-mm"));
            break;
        case HOURLY_ROLLOVER:
            setDatePattern(QLatin1String("'.'yyyy-MM-dd-hh"));
            break;
        case HALFDAILY_ROLLOVER:
            setDatePattern(QLatin1String("'.'yyyy-MM-dd-a"));
            break;
        case DAILY_ROLLOVER:
            setDatePattern(QLatin1String("'.'yyyy-MM-dd"));
            break;
        case WEEKLY_ROLLOVER:
            setDatePattern(QLatin1String("'.'yyyy-ww"));
            break;
        case MONTHLY_ROLLOVER:
            setDatePattern(QLatin1String("'.'yyyy-MM"));
            break;
        default:
            Q_ASSERT_X(false, "DailyRollingFileAppender::setDatePattern()", "Invalid datePattern constant");
            setDatePattern(DAILY_ROLLOVER);
    };
}

void DailyRollingFileAppender::open()
{
    computeFrequency();
    if (!m_activeDatePattern.isEmpty())
    {
        computeRollOverTime();
        FileAppender::open();
    }
}

void DailyRollingFileAppender::write(const QString &message)
{
    if (QDateTime::currentDateTime() > m_rollOverTime)
        rollOver();
    FileAppender::write(message);
}

void DailyRollingFileAppender::computeFrequency()
{
    const DateTime start_time(QDate(1999, 1, 1), QTime(0, 0));
    const QString start_string = start_time.toString(m_datePattern);
    m_activeDatePattern.clear();

    if (start_string != static_cast<DateTime>(start_time.addSecs(60)).toString(m_datePattern))
        m_frequency = MINUTELY_ROLLOVER;
    else if (start_string != static_cast<DateTime>(start_time.addSecs(60 * 60)).toString(m_datePattern))
        m_frequency = HOURLY_ROLLOVER;
    else if (start_string != static_cast<DateTime>(start_time.addSecs(60 * 60 * 12)).toString(m_datePattern))
        m_frequency = HALFDAILY_ROLLOVER;
    else if (start_string != static_cast<DateTime>(start_time.addDays(1)).toString(m_datePattern))
        m_frequency = DAILY_ROLLOVER;
    else if (start_string != static_cast<DateTime>(start_time.addDays(7)).toString(m_datePattern))
        m_frequency = WEEKLY_ROLLOVER;
    else if (start_string != static_cast<DateTime>(start_time.addMonths(1)).toString(m_datePattern))
        m_frequency = MONTHLY_ROLLOVER;
    else {
        return;
    }
    m_activeDatePattern = m_datePattern;
}

void DailyRollingFileAppender::computeRollOverTime()
{
    QDateTime now = QDateTime::currentDateTime();
    QDate now_date = now.date();
    QTime now_time = now.time();
    QDateTime start;

    switch (m_frequency)
    {
        case MINUTELY_ROLLOVER:
            {
                start = QDateTime(now_date,
                                  QTime(now_time.hour(),
                                        now_time.minute(),
                                        0, 0));
                m_rollOverTime = start.addSecs(60);
            }
            break;
        case HOURLY_ROLLOVER:
            {
                start = QDateTime(now_date,
                                  QTime(now_time.hour(),
                                        0, 0, 0));
                m_rollOverTime = start.addSecs(60*60);
            }
            break;
        case HALFDAILY_ROLLOVER:
            {
                int hour = now_time.hour();
                if (hour >=  12)
                    hour = 12;
                else
                    hour = 0;
                start = QDateTime(now_date,
                                  QTime(hour, 0, 0, 0));
                m_rollOverTime = start.addSecs(60*60*12);
            }
            break;
        case DAILY_ROLLOVER:
            {
                start = QDateTime(now_date,
                                  QTime(0, 0, 0, 0));
                m_rollOverTime = start.addDays(1);
            }
            break;
        case WEEKLY_ROLLOVER:
            {
                // QT numbers the week days 1..7. The week starts on Monday.
                // Change it to being numbered 0..6, starting with Sunday.
                int day = now_date.dayOfWeek();
                if (day == Qt::Sunday)
                    day = 0;
                start = QDateTime(now_date,
                                  QTime(0, 0, 0, 0)).addDays(-1 * day);
                m_rollOverTime = start.addDays(7);
            }
            break;
        case MONTHLY_ROLLOVER:
            {
                start = QDateTime(QDate(now_date.year(),
                                        now_date.month(),
                                        1),
                                  QTime(0, 0, 0, 0));
                m_rollOverTime = start.addMonths(1);
            }
            break;
        default:
            {
                Q_ASSERT_X(false, "DailyRollingFileAppender::computeInterval()", "Invalid datePattern constant");
                m_rollOverTime = QDateTime::fromTime_t(0);
            }
    }

    m_rollOverSuffix = static_cast<DateTime>(start).toString(m_activeDatePattern);
}

QString DailyRollingFileAppender::frequencyToString() const
{
    QMetaEnum meta_enum = metaObject()->enumerator(metaObject()->indexOfEnumerator("DatePattern"));
    return QLatin1String(meta_enum.valueToKey(m_frequency));
}

void DailyRollingFileAppender::rollOver()
{
    QString roll_over_suffix = m_rollOverSuffix;
    computeRollOverTime();
    if (roll_over_suffix == m_rollOverSuffix)
        return;

    closeFile();

    QString target_file_name = file() + roll_over_suffix;
    QFile f(target_file_name);
    if (f.exists() && !removeFile(f))
        return;
    f.setFileName(file());
    if (!renameFile(f, target_file_name))
        return;
    openFile();
}
