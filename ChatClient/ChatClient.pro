#-------------------------------------------------
#
# Project created by QtCreator 2017-11-05T15:08:59
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    client.cpp \
    common.cpp \
    connectdialog.cpp

HEADERS  += mainwindow.h \
    common.h \
    client.h \
    connectdialog.h

FORMS    += mainwindow.ui \
    connectdialog.ui \
