#include "ToastDialog.h"
#include <QtGlobal>
#include <QLabel>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>

ToastDialog::ToastDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    init();
}

ToastDialog::~ToastDialog()
{

}


void ToastDialog::show(const QString &text, QWidget *parent)
{
    QScreen *screen = QGuiApplication::primaryScreen();

    ToastDialog *toast = new ToastDialog(parent);
    toast->setText(text);
    toast->adjustSize();
    toast->move((screen->size().width()-toast->width())/2, screen->size().height()*4/10);
    toast->showAnimation();
}

void ToastDialog::init()
{
    m_pLabel = new QLabel(this);
    m_pLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(0);
    l->addWidget(m_pLabel);
}

void ToastDialog::showAnimation(int timeout)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();

    QWidget::show();

    QTimer::singleShot(timeout, [&]{

        QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
        animation->setDuration(1000);
        animation->setStartValue(1);
        animation->setEndValue(0);
        animation->start();

        connect(animation, &QPropertyAnimation::finished, [&]{
            close();
            deleteLater();
        });

    });
}

void ToastDialog::setText(const QString &text)
{
    m_pLabel->setText(text);
}

