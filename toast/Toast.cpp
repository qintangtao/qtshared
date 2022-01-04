#include "Toast.h"
#include "ToastDialog.h"

Toast::Toast(QObject *parent) :
    QObject(parent)
{
    
}

Toast::~Toast()
{
}


void Toast::show(const QString &text)
{
    ToastDialog::show(text);
}
