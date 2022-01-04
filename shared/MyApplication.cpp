#include "MyApplication.h"
#include <QSettings>
#include <QTextCodec>
#include <QDebug>
#include "log4qt/propertyconfigurator.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
Q_LOGGING_CATEGORY(logApp, "log.app")
#else
#define qCDebug(X)  qDebug() << #X
#define qCWarning(X)  qCWarning() << #X
#endif

MyApplication::MyApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    initTextCodec();

#ifdef ENABLED_LOG4QT_INIT
    initLog4qt();
#endif
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
        QSettings s("F:\\work\\kang\\KangView-control\\bin\\qtlogging.ini" , QSettings::IniFormat);
#endif
        s.setIniCodec(QTextCodec::codecForName("utf-8"));
        s.beginGroup("settings");
        Log4Qt::PropertyConfigurator::configure(s);
        s.endGroup();
#endif
}

bool MyApplication::event(QEvent *e)
{
    qCDebug(logApp) << e->type();
    return QApplication::event(e);
}
