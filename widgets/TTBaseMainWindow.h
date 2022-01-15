#ifndef TTBASEMAINWINDOW_H
#define TTBASEMAINWINDOW_H

#include <QMainWindow>


class TTBaseMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TTBaseMainWindow(QWidget *parent = nullptr);
    ~TTBaseMainWindow();

    inline bool isLazyLoaded() const
    { return m_bLazyLoading; }

private slots:
    void on_toolButton_min_clicked();

    void on_toolButton_max_clicked();

    void on_toolButton_close_clicked();

protected:
    virtual void closeEvent(QCloseEvent* event);
    virtual void showEvent(QShowEvent *event);
    virtual void lazyLoading();

protected:
    void readSettings();
    void writeSettings();

private:
    bool m_bLazyLoading;
};

#endif // TTBASEMAINWINDOW_H
