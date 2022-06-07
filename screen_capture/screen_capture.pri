
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

QT += core
