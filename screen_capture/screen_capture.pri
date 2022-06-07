
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/Decoder
INCLUDEPATH += $$PWD/Decoder/include
DEPENDPATH += $$PWD/Decoder/include

HEADERS += \
    $$PWD/ScreenCapture.h \
    $$PWD/Decoder/H264Encoder.h \
    $$PWD/Decoder/include/x264.h

SOURCES += \
    $$PWD/ScreenCapture.cpp \
    $$PWD/Decoder/H264Encoder.cpp


win32: LIBS += -L$$PWD/Decoder/lib/ -llibx264
win32: LIBS += -lgdi32


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

QT += core
