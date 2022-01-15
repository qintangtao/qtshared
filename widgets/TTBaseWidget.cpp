#include "TTBaseWidget.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logUWB, "log.ui.widget.base")

TTBaseWidget::TTBaseWidget(QWidget *parent) :
    QWidget(parent),
    m_bLazyLoading(false)
{

}

TTBaseWidget::~TTBaseWidget()
{

}

void TTBaseWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    if (!m_bLazyLoading) {
        m_bLazyLoading = true;
        lazyLoading();
    }
}

void TTBaseWidget::lazyLoading()
{
    qCDebug(logUWB) << metaObject()->className();
}
