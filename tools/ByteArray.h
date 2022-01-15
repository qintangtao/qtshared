#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <QObject>
#include <QByteArray>
#include <QString>

unsigned int ByteArray2Int(const QByteArray &byte);

QByteArray hexString2ByteArray(QString HexString);

#endif // BYTEARRAY_H
