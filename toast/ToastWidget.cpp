#include "ToastWidget.h"
#include <QtGlobal>
#include <QLabel>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>

ToastWidget::ToastWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_pLabel = new QLabel(this);
    m_pLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(0);
    l->addWidget(m_pLabel);
}

ToastWidget::~ToastWidget()
{

}


void ToastWidget::show(const QString &text, QWidget *parent)
{
    QScreen *screen = QGuiApplication::primaryScreen();

    ToastWidget *toast = new ToastWidget(parent);
    toast->setText(text);
    toast->adjustSize();
    toast->move((screen->size().width()-toast->width())/2, screen->size().height()*4/10);
    toast->showAnimation();
}


void ToastWidget::showAnimation(int timeout)
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

void ToastWidget::setText(const QString &text)
{
    m_pLabel->setText(text);
}

void ToastWidget::paintEvent(QPaintEvent *event)
{

#if 0
    QPainter paint(this);
    paint.begin(this);
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.setPen(Qt::NoPen);
    paint.setBrush(QBrush(QColor(255,255,255,0), Qt::SolidPattern));
    paint.drawRect(0,0,width(),height());
    paint.end();
#else
    QWidget::paintEvent(event);
#endif
}
