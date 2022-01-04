#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#include <QtWidgets/QApplication>

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(logApp)
#endif

class MyApplication : public QApplication
{
    Q_OBJECT

public:
    explicit MyApplication(int &argc, char **argv);
    ~MyApplication();

protected:
    void initTextCodec();
    void initLog4qt();

    bool event(QEvent *) override;

};

#endif // MYAPPLICATION_H
