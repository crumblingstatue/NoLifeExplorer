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


SOURCES += main.cpp \
           mainwindow.cpp \
           Sound.cpp \
           soundplayerwidget.cpp \
           NoLifeNx/Audio.cpp \
           NoLifeNx/Bitmap.cpp \
           NoLifeNx/File.cpp \
           NoLifeNx/NX.cpp \
           NoLifeNx/Node.cpp \
           NoLifeNx/lz4.cpp \

HEADERS  += mainwindow.h \
            Sound.h \
            NodeItem.h \
            soundplayerwidget.h \
            NoLifeNx/Audio.hpp \
            NoLifeNx/Bitmap.hpp \
            NoLifeNx/File.hpp \
            NoLifeNx/NX.hpp \
            NoLifeNx/Node.hpp \
            NoLifeNx/lz4.hpp

FORMS    += mainwindow.ui
LIBS += -lsfml-audio -lsfml-system -lmpg123
