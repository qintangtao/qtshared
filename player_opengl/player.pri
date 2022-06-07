
INCLUDEPATH += $$PWD

#DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/VideoDecoder.h \
    $$PWD/OpenGLPlayer.h

SOURCES += \
    $$PWD/VideoDecoder.cpp \
    $$PWD/OpenGLPlayer.cpp

LIBS += -L$$PWD/../libs/ffmpeg/lib/ -lhi_h264dec_w \
        -L$$PWD/../libs/ffmpeg/lib/ -lavcodec \
        -L$$PWD/../libs/ffmpeg/lib/ -lavdevice \
        -L$$PWD/../libs/ffmpeg/lib/ -lavfilter \
        -L$$PWD/../libs/ffmpeg/lib/ -lavformat \
        -L$$PWD/../libs/ffmpeg/lib/ -lavutil \
        -L$$PWD/../libs/ffmpeg/lib/ -lpostproc \
        -L$$PWD/../libs/ffmpeg/lib/ -lswresample \
        -L$$PWD/../libs/ffmpeg/lib/ -lswscale

INCLUDEPATH += $$PWD/../libs/ffmpeg/include

QT += widgets
