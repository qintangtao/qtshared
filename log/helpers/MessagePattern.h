#ifndef MESSAGEPATTERN_H
#define MESSAGEPATTERN_H

#include <QtGlobal>
#include <QString>


void qSetMessagePattern(const QString &pattern);

QString qFormatLogMessage(QtMsgType type, const QString &str);


#endif // MESSAGEPATTERN_H
