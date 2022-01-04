#include "Properties.h"
#include <QTextStream>
#include <QStringList>
#include <QSettings>

void Properties::load(QIODevice *pDevice)
{
    const QLatin1Char append_char(msEscapeChar);

    if (!pDevice)
    {
        return;
    }

    QTextStream stream(pDevice);
    QString line;
    int line_number = 0;
    QString property;
    int property_start_line = 1;

    do {
        line = trimLeft(stream.readLine());
        line_number++;

        if (!line.isEmpty() && line.at(line.length() - 1) == append_char)
            property += line.left(line.length() - 1);
        else
        {
            property += line;
            parseProperty(property, property_start_line);
            property.clear();
            property_start_line = line_number + 1;
        }
    }
    while (!line.isNull());
}


void Properties::load(const QSettings &rSettings)
{
    QStringList keys = rSettings.childKeys();
    QString key, vale;
    Q_FOREACH(key, keys)
    {
        QVariant var = rSettings.value(key);
        if (var.type() == QVariant::StringList)
            vale = var.toStringList().join(",");
        else
            vale = var.toString();
        insert(key, vale);
    }
}


QString Properties::property(const QString &rKey) const
{
    if (contains(rKey))
    {
        QString value = this->value(rKey);
        if (value.isNull())
            return QString(QLatin1String(""));
        else
            return value;
    }

    if (mpDefaultProperties)
        return mpDefaultProperties->property(rKey);
    else
        return QString();
}


QString Properties::property(const QString &rKey,
                             const QString &rDefaultValue) const
{
    QString value = property(rKey);
    if (value.isNull())
        return rDefaultValue;
    else
        return value;
}


QStringList Properties::propertyNames() const
{
    QStringList default_keys;
    if (mpDefaultProperties)
        default_keys = mpDefaultProperties->propertyNames();

    QStringList keys = this->keys();
    QString key;
    Q_FOREACH(key, default_keys)
        if (!keys.contains(key))
            keys << key;

    return keys;
}


void Properties::parseProperty(const QString &rProperty,
                               int line)
{
    Q_ASSERT_X(rProperty == trimLeft(rProperty), "parseProperty()", "rProperty has leading spaces");

    enum State
    {
        KEY_STATE,
        KEYSPACE_STATE,
        SPACEVALUE_STATE,
        VALUE_STATE,
        KEYESCAPE_STATE,
        VALUEESCAPE_STATE,
        UNICODEESCAPE_STATE
    };
    const QString value_escape_codes =QLatin1String(msValueEscapeCodes);
    const QString value_escape_chars = QLatin1String(msValueEscapeChars);
    Q_ASSERT_X(value_escape_codes.length() == value_escape_chars.length(), "parseProperty()", "Value escape sequence character definition does not map");
    const QString key_escape_codes = QLatin1String(msKeyEscapeCodes);
    const QString key_escape_chars = QLatin1String(msKeyEscapeChars);
    Q_ASSERT_X(key_escape_codes.length() == key_escape_chars.length(), "parseProperty()", "Key escape sequence character definition does not map");

    if (rProperty.isEmpty())
        return;

    int i = 0;
    QChar c;
    char ch;
    State state = KEY_STATE;
    QString key;
    QString value;
    QString *p_string = &key;
    uint ucs;
    int ucs_digits;
    while (i < rProperty.length())
    {
        c = rProperty.at(i);
        ch = c.toLatin1();

        switch (state)
        {
            case KEY_STATE:
                if (ch == '!' || ch == '#' )
                    return;
                else if (c.isSpace())
                {
                    p_string = &value;
                    state = KEYSPACE_STATE;
                }
                else if (ch == '=' || ch == ':')
                {
                    p_string = &value;
                    state = SPACEVALUE_STATE;
                }
                else if (ch == msEscapeChar)
                    state = KEYESCAPE_STATE;
                else
                    *p_string += c;
                break;
            case KEYSPACE_STATE:
                if (ch == '=' || ch == ':')
                    state = SPACEVALUE_STATE;
                else if (!c.isSpace())
                {
                    *p_string += c;
                    state = VALUE_STATE;
                }
                break;
            case SPACEVALUE_STATE:
                if (!c.isSpace())
                {
                    *p_string += c;
                    state = VALUE_STATE;
                }
                break;
            case VALUE_STATE:
                if (ch == msEscapeChar)
                    state = VALUEESCAPE_STATE;
                else
                    *p_string += c;
                break;
            case KEYESCAPE_STATE:
            {
                int convert = key_escape_codes.indexOf(c);
                if (convert >= 0)
                    *p_string += key_escape_chars.at(convert);
                else
                {
                    *p_string += c;
                }
                state = KEY_STATE;
                break;
            }
            case VALUEESCAPE_STATE:
            {
                int convert = value_escape_codes.indexOf(c);
                if (convert >= 0)
                {
                    *p_string += value_escape_chars.at(convert);
                    state = VALUE_STATE;
                }
                else if (ch == 'u')
                {
                    ucs = 0;
                    ucs_digits = 0;
                    state = UNICODEESCAPE_STATE;
                }
                else
                {
                    *p_string += c;
                    state = VALUE_STATE;
                }
                break;
            }
            case UNICODEESCAPE_STATE:
            {
                int hex = hexDigitValue(c);
                if (hex >= 0)
                {
                    ucs = ucs * 16 + hex;
                    ucs_digits++;
                    if (ucs_digits == 4 || i == rProperty.length() - 1)
                    {
                        *p_string += QChar(ucs);
                        state = VALUE_STATE;
                    }
                }
                else
                {
                    if (ucs_digits > 0)
                        *p_string += QChar(ucs);
                    state = VALUE_STATE;
                    continue;
                }
                break;
            }
            default:
                Q_ASSERT_X(false, "Properties::parseProperty()", "Unknown state constant");
                return;
        }
        i++;
    }

   // if (key.isEmpty() && !value.isEmpty())
     //   logger()->warn("Found value with no key in property starting at line %1", line);

    //logger()->trace("Loaded property '%1' : '%2'", key, value);
    insert(key, value);
}


int Properties::hexDigitValue(const QChar &rDigit)
{
    bool ok;
    int result = QString(rDigit).toInt(&ok, 16);
    if (!ok)
        return -1;
    else
        return result;
}


QString Properties::trimLeft(const QString &rLine)
{
    int i = 0;
    while (i < rLine.length() && rLine.at(i).isSpace())
        i++;
    return rLine.right(rLine.length() - i);
}


const char Properties::msEscapeChar ='\\';
const char *Properties::msValueEscapeCodes = "tnr\\\"\' ";
const char *Properties::msValueEscapeChars = "\t\n\r\\\"\' ";
const char *Properties::msKeyEscapeCodes = " :=";
const char *Properties::msKeyEscapeChars = " :=";
