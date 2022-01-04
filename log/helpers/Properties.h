#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QHash>

class QIODevice;
class QSettings;

class Properties : public QHash<QString, QString>
{
public:
    Properties(Properties *pDefaultProperties = 0);
    // virtual ~Properties(); // Use compiler default
    // Properties(const Properties &rOther); // Use compiler default
    // Properties &operator=(const Properties &rOther); // Not implemented

public:
    Properties *defaultProperties() const;
    QString property(const QString &rKey) const;
    QString property(const QString &rKey,
                     const QString &rDefaultValue) const;
    void setDefaultProperties(Properties *pDefault);
    void setProperty(const QString &rKey,
                     const QString &rValue);

    void load(QIODevice *pDevice);
    void load(const QSettings &rSettings);

    QStringList propertyNames() const;

private:
    void parseProperty(const QString &rProperty,
                       int line);
    static int hexDigitValue(const QChar &rDigit);
    static QString trimLeft(const QString &rString);

private:
    Properties *mpDefaultProperties;
    static const char msEscapeChar;
    static const char *msValueEscapeCodes;
    static const char *msValueEscapeChars;
    static const char *msKeyEscapeCodes;
    static const char *msKeyEscapeChars;
};

inline Properties::Properties(Properties *pDefaultProperties) :
        mpDefaultProperties(pDefaultProperties)
{}

inline Properties *Properties::defaultProperties() const
{	return mpDefaultProperties;	}

inline void Properties::setDefaultProperties(Properties *pDefaultProperties)
{	mpDefaultProperties = pDefaultProperties;	}

inline void Properties::setProperty(const QString &rKey,
                                    const QString &rValue)
{   insert(rKey, rValue);	}

#endif // PROPERTIES_H
