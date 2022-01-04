#ifndef OPTIONCONVERTER_H
#define OPTIONCONVERTER_H
#include <QObject>

#include "Properties.h"

class OptionConverter
{
private:
    explicit OptionConverter();
    OptionConverter(const OptionConverter &rOther); // Not implemented
    // virtual ~OptionConverter(); // Use compiler default
    OptionConverter &operator=(const OptionConverter &rOther); // Not implemented

public:

    static QString findAndSubst(const Properties &rProperties,
                                        const QString &rKey);

    static QString classNameJavaToCpp(const QString &rClassName);

    static bool toBoolean(const QString &rOption,
                                  bool *p_ok = 0);

    static bool toBoolean(const QString &rOption,
                          bool default_value);

    static qint64 toFileSize(const QString &rOption,
                             bool *p_ok = 0);

    static int toInt(const QString &rOption,
                     bool *p_ok = 0);

    static int toTarget(const QString &rOption, bool *p_ok);

};

#endif // OPTIONCONVERTER_H
