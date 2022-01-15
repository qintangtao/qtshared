#include "TTPushButton.h"
#include <QVariant>
#include <QStyle>

TTPushButton::TTPushButton(QWidget *parent) :
    QPushButton(parent)
{
    this->setProperty("status", "unchecked");
    connect(this, &QPushButton::toggled, [=](bool flag) {
        flag ? setProperty("status", "checked") : setProperty("status", "unchecked");
        style()->polish(this);
    });
}

TTPushButton::~TTPushButton()
{
}
