#include "MyStyle.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QColor>
#include <QPalette>

MyStyle::MyStyle()
{

}

MyStyle::~MyStyle()
{

}

void MyStyle::setStyle(const QString &styleName)
{
    //加载样式表
    QFile file(QString(":/resource/qss/%1.css").arg(styleName));
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}
