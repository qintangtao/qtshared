#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <QThread>

class ScreenCapture : public QThread
{
    Q_OBJECT
public:
    explicit ScreenCapture(QObject *parent = 0);

protected:
    virtual void run();


};

#endif // SCREENCAPTURE_H
