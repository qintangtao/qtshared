#ifndef MYSTYLE_H
#define MYSTYLE_H
#include <QString>

class MyStyle
{
public:

   static void setStyle(const QString &styleName);

private:
    explicit MyStyle();
    ~MyStyle();
};

#endif // MYSTYLE_H
