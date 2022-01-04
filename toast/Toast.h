#ifndef Toast_H
#define Toast_H

#include <QObject>

class Toast : public QObject
{
    Q_OBJECT

private:
    explicit Toast(QObject *parent = nullptr);
    ~Toast();

public:
    static void show(const QString &text);

};

#endif // Toast_H
