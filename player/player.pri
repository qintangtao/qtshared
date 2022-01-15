
INCLUDEPATH += $$PWD

#DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/BCDecoderPlayer.h

SOURCES += \
    $$PWD/BCDecoderPlayer.cpp

LIBS += -L$$PWD/Player/lib/ -lhi_h264dec_w \
        -L$$PWD/Player/lib/ -lavcodec \
        -L$$PWD/Player/lib/ -lavdevice \
        -L$$PWD/Player/lib/ -lavfilter \
        -L$$PWD/Player/lib/ -lavformat \
        -L$$PWD/Player/lib/ -lavutil \
        -L$$PWD/Player/lib/ -lpostproc \
        -L$$PWD/Player/lib/ -lswresample \
        -L$$PWD/Player/lib/ -lswscale

INCLUDEPATH += $$PWD/Player/include

QT += widgets
