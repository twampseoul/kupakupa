#-------------------------------------------------
#
# Project created by QtCreator 2014-07-08T16:55:17
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kupa-gui-2
TEMPLATE = app


SOURCES += main.cpp\
        kupagui.cpp \
    tinystr.cpp \
    tinyxmlparser.cpp \
    tinyxmlerror.cpp \
    tinyxml.cpp \
    parseXml.cpp

HEADERS  += kupagui.h \
    tinystr.h \
    tinyxml.h \
    parseXml.h

FORMS    += kupagui.ui

OTHER_FILES +=

RESOURCES += \
    kupa-gui-2.qrc
