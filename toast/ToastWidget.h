#ifndef ToastWidget_H
#define ToastWidget_H

#include <QWidget>


class QLabel;

class ToastWidget : public QWidget
{
    Q_OBJECT

private:
    explicit ToastWidget(QWidget *parent = NULL);
    ~ToastWidget();

public:
    static void show(const QString &text, QWidget *parent=0);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:

    void showAnimation(int timeout=2000);

    void setText(const QString &text);

private:
    QLabel *m_pLabel;
};

#endif // ToastWidget_H
