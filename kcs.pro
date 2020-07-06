#-------------------------------------------------
#
# Project created by QtCreator 2019-06-18T11:33:38
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kcs
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on you              r compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    cardreader.cpp \


HEADERS  += mainwindow.h \
    cardreader.h \




FORMS    += mainwindow.ui

LIBS +=-L\usr\locl\lib -lwiringPi


DISTFILES += \
    read.py \
    res/key.gif \
    res/unlock.gif \
    res/unlock1.gif \
    res/background.jpg \
    res/background1.jpg \
    res/background2.jpg \
    res/dd1.jpg \
    res/background.png \
    res/cancel.png \
    res/fav.png \
    res/gunsel_logo.png \
    res/gunsel_yatay.png \
    res/i3.png \
    res/imiev.png \
    res/in.png \
    res/key.png \
    res/logo-min.png \
    res/out.png \
    res/vitz.png \
    res/vitz9.png \
    res/vitz3.png

