#include "OptionConverter.h"
#include "appender/ConsoleAppender.h"

QString OptionConverter::findAndSubst(const Properties &rProperties,
                                          const QString &rKey)
{
    QString value = rProperties.property(rKey);
    if (value.isNull())
        return value;

    const QString begin_subst = QLatin1String("${");
    const QString end_subst = QLatin1String("}");
    const int begin_length = begin_subst.length();
    const int end_length = end_subst.length();

    QString result = QLatin1String("");

    int i = 0;
    int begin;
    int end;
    while (i < value.length())
    {
        begin = value.indexOf(begin_subst, i);
        if (begin == -1)
        {
            result += value.mid(i);
            i = value.length();
        }
        else
        {
            result += value.mid(i, begin - i);
            end = value.indexOf(end_subst, i + begin_length);
            if (end == -1)
            {
                return result;
            }
            else
            {
                result += findAndSubst(rProperties, value.mid(begin + begin_length, end - begin - end_length - 1));
                i = end + end_length;
            }
        }
    }
    return result;
}


QString OptionConverter::classNameJavaToCpp(const QString &rClassName)
{
    const QLatin1String java_class_delimiter(".");
    const QLatin1String cpp_class_delimiter("::");

    QString result = rClassName;
    return result.replace(java_class_delimiter, cpp_class_delimiter);
}

bool OptionConverter::toBoolean(const QString &rOption,
                                    bool *p_ok)
{
    const QLatin1String str_true("true");
    const QLatin1String str_enabled("enabled");
    const QLatin1String str_one("1");
    const QLatin1String str_false("false");
    const QLatin1String str_disabled("disabled");
    const QLatin1String str_zero("0");

    if (p_ok)
        *p_ok = true;
    QString s = rOption.trimmed().toLower();
    if (s == str_true || s == str_enabled || s == str_one)
        return true;
    if (s == str_false || s == str_disabled || s == str_zero)
        return false;

    if (p_ok)
        *p_ok = false;

    return false;
}


bool OptionConverter::toBoolean(const QString &rOption,
                                bool default_value)
{
    bool ok;
    bool result = toBoolean(rOption, &ok);
    if (ok)
        return result;
    else
        return default_value;
}

qint64 OptionConverter::toFileSize(const QString &rOption,
                                   bool *p_ok)
{
    if (p_ok)
        *p_ok = false;
    QString s = rOption.trimmed().toLower();
    qint64 f = 1;
    int i;
    i = s.indexOf(QLatin1String("kb"));
    if (i >= 0)
        f = 1024;
    else
    {
        i = s.indexOf(QLatin1String("mb"));
        if (i >= 0)
            f = 1024 * 1024;
        else
        {
            i = s.indexOf(QLatin1String("gb"));
            if (i >= 0)
                f = 1024 * 1024 * 1024;
        }
    }
    if (i < 0)
        i = s.length();
    bool ok;
    qint64 value = s.left(i).toLongLong(&ok);
    if (!ok || value < 0 || s.length() > i + 2)
    {
        return 0;
    }
    if (p_ok)
        *p_ok = true;
    return value * f;
}


int OptionConverter::toInt(const QString &rOption,
                           bool *p_ok)
{
    int value = rOption.trimmed().toInt(p_ok);
    if (*p_ok)
        return value;

    return 0;
}


int OptionConverter::toTarget(const QString &rOption, bool *p_ok)
{
    const QLatin1String java_stdout("system.out");
    const QLatin1String cpp_stdout("stdout_target");
    const QLatin1String qt_stdout("STDOUT_TARGET");
    const QLatin1String java_stderr("system.err");
    const QLatin1String cpp_stderr("stderr_target");
    const QLatin1String qt_stderr("STDERR_TARGET");

    if (p_ok)
        *p_ok = true;

    QString s = rOption.trimmed().toLower();
    if (s == java_stdout || s == cpp_stdout || s == qt_stdout)
    return ConsoleAppender::STDOUT_TARGET;
    if (s == java_stderr || s == cpp_stderr || s == qt_stderr)
    return ConsoleAppender::STDERR_TARGET;

    if (p_ok)
       *p_ok = false;

    return ConsoleAppender::STDOUT_TARGET;
}
