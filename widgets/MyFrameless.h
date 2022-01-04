#ifndef MyFRAMELESS_H
#define MyFRAMELESS_H

#include <QObject>
#include <QPoint>

class QMouseEvent;
class QHoverEvent;

class MyFrameless : public QObject
{
    Q_OBJECT
public:
    MyFrameless(QObject  *parent, QWidget *shadow=0);
    ~MyFrameless();

    enum RESIZEPOS {
        UNRESIZE,   // 不可拉伸
        RESIZELT,   // 左上角
        RESIZEL,    // 左侧
        RESIZELB,   // 左下角
        RESIZET,    // 上
        RESIZEB,    // 下
        RESIZERT,   // 右上角
        RESIZER,    // 右
        RESIZERB    // 右下角
    };

    void setMovable(bool movable);
    void setResizable(bool resizable);
    void setBorderWidth(uint width);

    QWidget* widget() const;

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
    // 处理鼠标按下
    void handleMousePressEvent(QMouseEvent *event);
    // 处理鼠标释放
    void handleMouseReleaseEvent(QMouseEvent *event);
    // 处理鼠标移动
    void handleMouseMoveEvent(QMouseEvent *event);
    // 处理鼠标离开
    void handleLeaveEvent(QEvent *event);
    // 处理鼠标进入
    void handleHoverMoveEvent(QHoverEvent *event);

    // 更新鼠标样式
    void updateCursorShape(const QPoint &pt);
    // 重置窗体大小
    void resizeWidget(const QPoint &pt);
    // 移动窗体
    void moveWidget(const QPoint &pt);

private:
    Qt::WindowFlags m_windowFlags;
    RESIZEPOS m_eResizePos;
    bool m_bLeftButtonPressed;
    bool m_bResizable;
    bool m_bMovable;
    int m_nShadowWidth;
    int m_nBorderWidth;
    QPoint m_ptDragPos;
};

#endif // MyFRAMELESS_H
