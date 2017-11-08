#-------------------------------------------------
#
# Project created by QtCreator 2017-11-06T10:36:04
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = Common
TEMPLATE = lib

DEFINES += COMMON_LIBRARY

SOURCES += common.cpp

HEADERS += common.h\
        common_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
