
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/appender
#DEPENDPATH += $$PWD

DEFINES += QT_MESSAGELOGCONTEXT

HEADERS += \
    $$PWD/LogDef.h \
    $$PWD/LogManager.h \
    $$PWD/Logger.h \
    $$PWD/LoggerRepository.h \
    $$PWD/BasicConfigurator.h \
    $$PWD/PropertyConfigurator.h \
    $$PWD/helpers/ClassLogger.h \
    $$PWD/helpers/OptionConverter.h \
    $$PWD/helpers/DateTime.h \
    $$PWD/helpers/Factory.h \
    $$PWD/helpers/Properties.h \
    $$PWD/appender/Appender.h \
    $$PWD/appender/WriterAppender.h \
    $$PWD/appender/ConsoleAppender.h \
    $$PWD/appender/DebugAppender.h \
    $$PWD/appender/NullAppender.h \
    $$PWD/appender/FileAppender.h \
    $$PWD/appender/ListAppender.h \
    $$PWD/appender/RollingFileAppender.h \
    $$PWD/appender/DailyRollingFileAppender.h


SOURCES += \
    $$PWD/LogManager.cpp \
    $$PWD/Logger.cpp \
    $$PWD/LoggerRepository.cpp \
    $$PWD/BasicConfigurator.cpp \
    $$PWD/PropertyConfigurator.cpp \
    $$PWD/helpers/ClassLogger.cpp \
    $$PWD/helpers/OptionConverter.cpp \
    $$PWD/helpers/DateTime.cpp \
    $$PWD/helpers/Factory.cpp \
    $$PWD/helpers/Properties.cpp \
    $$PWD/appender/WriterAppender.cpp \
    $$PWD/appender/ConsoleAppender.cpp \
    $$PWD/appender/DebugAppender.cpp \
    $$PWD/appender/NullAppender.cpp \
    $$PWD/appender/ListAppender.cpp \
    $$PWD/appender/FileAppender.cpp \
    $$PWD/appender/RollingFileAppender.cpp \
    $$PWD/appender/DailyRollingFileAppender.cpp

lessThan(QT_MAJOR_VERSION, 5) {
HEADERS += \
    $$PWD/helpers/MessagePattern.h

SOURCES += \
    $$PWD/helpers/MessagePattern.cpp
}

QT += core

