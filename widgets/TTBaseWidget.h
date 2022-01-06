#ifndef TTBASEWIDGET_H
#define TTBASEWIDGET_H

#include <QWidget>

class TTBaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TTBaseWidget(QWidget *parent = nullptr);
    ~TTBaseWidget();

    inline bool isLazyLoaded() const
    { return m_bLazyLoading; }

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void lazyLoading();

private:
    bool m_bLazyLoading;
};

#endif // TTBASEWIDGET_H
