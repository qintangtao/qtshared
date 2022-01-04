#include "MyToolButton.h"
#include <QVariant>
#include <QStyle>

MyToolButton::MyToolButton(QWidget *parent) :
    QToolButton(parent)
{
    this->setProperty("status", "unchecked");
    connect(this, &QToolButton::toggled, [=](bool flag) {
        flag ? setProperty("status", "checked") : setProperty("status", "unchecked");
        style()->polish(this);
    });
}

MyToolButton::~MyToolButton()
{
}
