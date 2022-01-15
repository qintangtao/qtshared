#ifndef ToastDialog_H
#define ToastDialog_H

#include <QDialog>


class QLabel;

class ToastDialog : public QDialog
{
    Q_OBJECT

private:
    explicit ToastDialog(QWidget *parent = NULL);
    ~ToastDialog();

public:
    static void show(const QString &text, QWidget *parent=0);

protected:

    void init();

    void showAnimation(int timeout=2000);

    void setText(const QString &text);

protected:
    QLabel *m_pLabel;

private:
    Q_DISABLE_COPY(ToastDialog)
};

#endif // ToastDialog_H
