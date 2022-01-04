#include "MyFrameless.h"
#include <QWidget>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QLayout>

MyFrameless::MyFrameless(QObject *parent, QWidget *shadow) :
    QObject(parent)
{
    m_eResizePos = UNRESIZE;
    m_bLeftButtonPressed = false;
    m_bResizable = true;
    m_bMovable = true;
    m_nShadowWidth = 0;
    m_nBorderWidth = 5;

    QWidget* widget =this->widget();
    m_windowFlags = widget->windowFlags();
    widget->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    widget->setMouseTracking(true);
    widget->setAttribute(Qt::WA_Hover, true);
    widget->installEventFilter(this);
#if 1

    if(shadow != NULL) {
         widget->setAttribute(Qt::WA_TranslucentBackground, true);
         //实例阴影shadow
         QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
         //设置阴影距离
         shadowEffect->setOffset(0, 0);
         //设置阴影颜色
         shadowEffect->setColor(QColor("#444444"));
         //设置阴影圆角
         shadowEffect->setBlurRadius(20);
         //给嵌套QWidget设置阴影
         shadow->setGraphicsEffect(shadowEffect);
         //给垂直布局器设置边距(此步很重要, 设置宽度为阴影的宽度)
         m_nShadowWidth = widget->layout()->margin();
         if(m_nShadowWidth < 5) {
             m_nShadowWidth = 9;
             widget->layout()->setMargin(m_nShadowWidth);
         }

         m_nBorderWidth += m_nShadowWidth;
	}
#endif
}

MyFrameless::~MyFrameless()
{
    QWidget* widget =this->widget();
    widget->removeEventFilter(this);
    widget->setMouseTracking(false);
    widget->setWindowFlags(m_windowFlags);
    widget->setAttribute(Qt::WA_Hover, false);
}

void MyFrameless::setMovable(bool movable)
{
    m_bMovable = movable;
}

void MyFrameless::setResizable(bool resizable)
{
    m_bResizable = resizable;
}

void MyFrameless::setBorderWidth(uint width)
{
    if(width > 0)
    {
        m_nBorderWidth = m_nShadowWidth + width;
    }
}

QWidget* MyFrameless::widget() const
{
    return static_cast<QWidget*>(parent());
}

bool MyFrameless::eventFilter(QObject *obj, QEvent *event)
{
    //qDebug() << "eventFilter" << event->type();
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
            handleMousePressEvent(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::MouseButtonRelease:
            handleMouseReleaseEvent(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::MouseMove:
            handleMouseMoveEvent(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::Leave:
            handleLeaveEvent(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::HoverMove:
            handleHoverMoveEvent(static_cast<QHoverEvent*>(event));
            break;
        default:
            return QObject::eventFilter(obj, event);
    }

    return true;
}

void MyFrameless::handleMousePressEvent(QMouseEvent *event)
{
   //qDebug() << "handleMousePressEvent";
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = true;
        m_ptDragPos = event->globalPos() - this->widget()->frameGeometry().topLeft();
        updateCursorShape(event->globalPos());
    }
}

void MyFrameless::handleMouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "handleMouseReleaseEvent";
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = false;
        if(m_eResizePos != UNRESIZE)
        {
            m_eResizePos = UNRESIZE;
            this->widget()->unsetCursor();
        }
    }
}

void MyFrameless::handleMouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "handleMouseMoveEvent";
    if(m_bLeftButtonPressed)
    {
        if(m_bResizable && m_eResizePos != UNRESIZE)
        {
            resizeWidget(event->globalPos());
        }
        else if (m_bMovable)
        {
            moveWidget(event->globalPos());
        }
    }
    else if (m_bResizable)
    {
        updateCursorShape(event->globalPos());
    }
}

void MyFrameless::handleLeaveEvent(QEvent *)
{
    //qDebug() << "handleLeaveEvent";
    if (!m_bLeftButtonPressed && m_eResizePos != UNRESIZE)
    {
            m_eResizePos = UNRESIZE;
            this->widget()->unsetCursor();
    }
}

void MyFrameless::handleHoverMoveEvent(QHoverEvent *event)
{
    //qDebug() << "handleHoverMoveEvent";
    if (m_bResizable && !m_bLeftButtonPressed)
    {
        updateCursorShape(this->widget()->mapToGlobal(event->pos()));
    }
}


void MyFrameless::updateCursorShape(const QPoint &pt)
{
    QWidget* widget = this->widget();
    if (widget->isFullScreen() || widget->isMaximized())
    {
        return;
    }

    QRect rect = widget->frameGeometry();

    int nltx = pt.x() - rect.x();
    int nlty = pt.y() - rect.y();

    // x在修改范围时
    if (nltx <= m_nBorderWidth) {
        if (nlty <= m_nBorderWidth) {
            // ↖
            m_eResizePos = RESIZELT;
            widget->setCursor(Qt::SizeFDiagCursor);
        } else if (qAbs(nlty-rect.height()) <= m_nBorderWidth) {
            // ↙
            m_eResizePos = RESIZELB;
            widget->setCursor(Qt::SizeBDiagCursor);
        } else {
            // ←
            m_eResizePos = RESIZEL;
            widget->setCursor(Qt::SizeHorCursor);
        }
    } else if (qAbs(nltx-rect.width()) <= m_nBorderWidth) {
        if (nlty <= m_nBorderWidth) {
            // ↗
            m_eResizePos = RESIZERT;
            widget->setCursor(Qt::SizeBDiagCursor);
        } else if (qAbs(nlty-rect.height()) <= m_nBorderWidth) {
            // ↘
            m_eResizePos = RESIZERB;
           widget->setCursor(Qt::SizeFDiagCursor);
        } else {
            // →
            m_eResizePos = RESIZER;
            widget->setCursor(Qt::SizeHorCursor);
        }
    } else {
        if (nlty <= m_nBorderWidth) {
            // ↑
            m_eResizePos = RESIZET;
            widget->setCursor(Qt::SizeVerCursor);
        } else if (qAbs(nlty-rect.height()) <= m_nBorderWidth) {
            // ↓
            m_eResizePos = RESIZEB;
            widget->setCursor(Qt::SizeVerCursor);
        } else {
            // 不拉伸
            m_eResizePos = UNRESIZE;
            widget->setCursor(Qt::ArrowCursor);
        }
    }
}

void MyFrameless::resizeWidget(const QPoint &pt)
{
    QWidget* widget = this->widget();
    QRect origRect = widget->frameGeometry();
    int minWidth = widget->minimumWidth();
    int minHeight = widget->minimumHeight();

    int left, top, right, bottom;
    origRect.getCoords(&left, &top, &right, &bottom);

    switch(m_eResizePos)
    {
    case RESIZELT:
        left = pt.x();
        top = pt.y();
        break;
    case RESIZELB:
        left = pt.x();
        bottom = pt.y();
        break;
    case RESIZEL:
        left = pt.x();
        break;
    case RESIZERT:
        right = pt.x();
        top = pt.y();
        break;
    case RESIZERB:
        right = pt.x();
        bottom = pt.y();
        break;
    case RESIZER:
        right = pt.x();
        break;
    case RESIZET:
        top = pt.y();
        break;
    case RESIZEB:
        bottom = pt.y();
        break;
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));
    //qDebug() << "resizeWidget" << m_eResizePos << newRect << newRect.isValid();
    if (newRect.isValid())
    {
           if (minWidth > newRect.width())
           {
               if (left != origRect.left())
                   newRect.setLeft(origRect.left());
               else
                   newRect.setRight(origRect.right());
           }
           if (minHeight > newRect.height())
           {
               if (top != origRect.top())
                   newRect.setTop(origRect.top());
               else
                   newRect.setBottom(origRect.bottom());
           }
           widget->setGeometry(newRect);
     }
}

void MyFrameless::moveWidget(const QPoint &pt)
{
	QWidget* widget = this->widget();
	if (widget->isFullScreen() || widget->isMaximized())
	{
		return;
	}
    this->widget()->move(pt - m_ptDragPos);
}
