#-------------------------------------------------
#
# Project created by QtCreator 2013-05-25T12:22:34
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NoLifeJukebox
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        NoLifeNx/NX.cpp\
        NoLifeNx/lz4.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
LIBS += -lbass
