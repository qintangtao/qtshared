#ifndef MYTOOLBUTTON_H
#define MYTOOLBUTTON_H

#include <QToolButton>



class MyToolButton : public QToolButton
{
    Q_OBJECT

public:
    explicit MyToolButton(QWidget *parent = nullptr);
    ~MyToolButton();

};

#endif // MYTOOLBUTTON_H
