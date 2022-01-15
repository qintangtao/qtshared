#ifndef TTPushButton_H
#define TTPushButton_H

#include <QPushButton>



class TTPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit TTPushButton(QWidget *parent = nullptr);
    ~TTPushButton();

};

#endif // TTPushButton_H
