
INCLUDEPATH += $$PWD

#DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/MyFrameless.h \
    $$PWD/flowlayout.h \
    $$PWD/MyToolButton.h  \
    $$PWD/MyEditButton.h \
    $$PWD/TTBaseWidget.h \
    $$PWD/TTPushButton.h  \
    $$PWD/TTBaseMainWindow.h

SOURCES += \
    $$PWD/MyFrameless.cpp \
    $$PWD/flowlayout.cpp \
    $$PWD/MyToolButton.cpp  \
    $$PWD/MyEditButton.cpp \
    $$PWD/TTBaseWidget.cpp \
    $$PWD/TTPushButton.cpp  \
    $$PWD/TTBaseMainWindow.cpp

FORMS +=

QT += widgets
