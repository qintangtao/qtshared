#include "TTBaseMainWindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QCloseEvent>

#include <QLoggingCategory>
#include <MyFrameless.h>

Q_LOGGING_CATEGORY(logUWBMW, "log.ui.widget.base.mainwindow")

TTBaseMainWindow::TTBaseMainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_bLazyLoading(false)
{
    new MyFrameless(this);
}

TTBaseMainWindow::~TTBaseMainWindow()
{
}


void TTBaseMainWindow::closeEvent(QCloseEvent* event)
{
    qCDebug(logUWBMW) <<  pos() << size();

    if (!isMinimized() && !isFullScreen())
        writeSettings();

    event->accept();
}

void TTBaseMainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    if (!m_bLazyLoading) {
        m_bLazyLoading = true;
        lazyLoading();
    }
}

void TTBaseMainWindow::lazyLoading()
{
    qCDebug(logUWBMW) << metaObject()->className();

    readSettings();
}

void TTBaseMainWindow::readSettings()
{
    QSettings settings(this);
    QRect geom = QApplication::desktop()->availableGeometry();
#if 0
    QPoint pos = settings.value(QLatin1String("pos"), QPoint(200, 200)).toPoint();
    QSize size = settings.value(QLatin1String("size"), QSize(2 * geom.width() / 3, 2 * geom.height() / 3)).toSize();
#else
    QPoint pos = settings.value(QLatin1String("pos"), QPoint(0, 0)).toPoint();
    QSize size = settings.value(QLatin1String("size"), QSize(0, 0)).toSize();
    if (size == QSize(0,0)) {
        showMaximized();
    } else {
        move(pos);
        resize(size);
    }
#endif
}

void TTBaseMainWindow::writeSettings()
{
    QSettings settings(this);   
    settings.setValue(QLatin1String("pos"), isMaximized() ? QPoint(0,0) : pos());
    settings.setValue(QLatin1String("size"), isMaximized() ? QSize(0,0) : size());
}

void TTBaseMainWindow::on_toolButton_min_clicked()
{
    showMinimized();
}

void TTBaseMainWindow::on_toolButton_max_clicked()
{
    if (isMaximized())
        showNormal();
    else
        showMaximized();
}

void TTBaseMainWindow::on_toolButton_close_clicked()
{
    close();
}
