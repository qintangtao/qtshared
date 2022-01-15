#include "MyApplication.h"
#include <QSettings>
#include <QTextCodec>
#include <QDebug>
#include <QFontDatabase>
#include "log4qt/propertyconfigurator.h"

Q_LOGGING_CATEGORY(logApp, "log.app")

MyApplication::MyApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    initTextCodec();

#ifndef DISABLE_LOG4QT_INIT
    initLog4qt();
#endif

    loadFont();
}

MyApplication::~MyApplication()
{

}

void MyApplication::initTextCodec()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("GBK");
#else
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#endif
}

void MyApplication::initLog4qt()
{
#if 0
        BasicConfigurator::configure();
#else
#ifdef QT_NO_DEBUG
        QSettings s("qtlogging.ini" , QSettings::IniFormat);
#else
        QSettings s("qtlogging.ini" , QSettings::IniFormat);
#endif
        s.setIniCodec(QTextCodec::codecForName("utf-8"));
        s.beginGroup("settings");
        Log4Qt::PropertyConfigurator::configure(s);
        s.endGroup();
#endif
}

void MyApplication::loadFont()
{
    //load font
    int index = QFontDatabase::addApplicationFont("msyh.ttf");
    if (-1 == index) {
        qCWarning(logApp) << "addApplicationFont" << index;
        return;
    }

    qCDebug(logApp) << index;

    QStringList fontList(QFontDatabase::applicationFontFamilies(index));
    if (fontList.isEmpty()) {
        qCWarning(logApp) << "fontList is empty" << index;
        return;
    }

    foreach(QString font, fontList) {
        qCDebug(logApp) << font;
    }

    QFont font_zh(fontList.at(0));
    font_zh.setBold(false);
    this->setFont(font_zh);
}


bool MyApplication::event(QEvent *e)
{
    qCDebug(logApp) << e->type();
    return QApplication::event(e);
}
