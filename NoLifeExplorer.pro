#-------------------------------------------------
#
# Project created by QtCreator 2013-05-25T12:22:34
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NoLifeExplorer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        NoLifeNx/NX.cpp\
        NoLifeNx/lz4.cpp \
    Sound.cpp \
    soundplayerwidget.cpp

HEADERS  += mainwindow.h \
    Sound.h \
    SoundItem.h \
    soundplayerwidget.h

FORMS    += mainwindow.ui
LIBS += -lsfml-audio -lsfml-system -lmpg123
