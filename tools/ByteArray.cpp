#include "ByteArray.h"
#include <QStringList>

unsigned int ByteArray2Int(const QByteArray &byte)
{
    if (byte.length() != 4)
        return 0;

    unsigned int addr = byte[0] & 0x000000FF;
    addr |= ((byte[1] << 8) & 0x0000FF00);
    addr |= ((byte[2] << 16) & 0x00FF0000);
    addr |= ((byte[3] << 24) & 0xFF000000);
    return addr;
}

QByteArray hexString2ByteArray(QString HexString)
{
   QByteArray ret;
   HexString = HexString.trimmed();
   HexString = HexString.simplified();
   if (!HexString.contains(QLatin1String(" ")))
   {
       int length = HexString.length();
       if (length % 2 == 0)
       {
           for (int i = 0; i < length; i += 2)
           {
               QString s = HexString.mid(i, 2);
               if (!s.isEmpty())
               {
                   bool ok;
                   char c = s.toInt(&ok, 16) & 0xFF;
                   if (ok)
                       ret.append(c);
                   else
                       return QByteArray();
               }
           }
       }
       else
       {
            return QByteArray();
       }
   }
   else {
       QStringList sl = HexString.split(QLatin1String(" "));
       foreach(QString s, sl) {
           if (!s.isEmpty())
           {
               bool ok;
               char c = s.toInt(&ok, 16) & 0xFF;
               if (ok)
                   ret.append(c);
               else
                   return QByteArray();
           }
       }
   }
   return ret;
}
