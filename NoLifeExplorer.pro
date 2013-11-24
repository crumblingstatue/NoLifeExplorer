#-------------------------------------------------
#
# Project created by QtCreator 2013-05-25T12:22:34
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11 -pedantic -Wall -Wextra -Wshadow

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NoLifeExplorer
TEMPLATE = app


SOURCES += main.cpp \
           mainwindow.cpp \
           Sound.cpp \
           soundplayerwidget.cpp \
           NoLifeNx/audio.cpp \
           NoLifeNx/bitmap.cpp \
           NoLifeNx/file.cpp \
           NoLifeNx/nx.cpp \
           NoLifeNx/node.cpp

HEADERS  += mainwindow.h \
            Sound.h \
            NodeItem.h \
            soundplayerwidget.h \
            NoLifeNx/audio.hpp \
            NoLifeNx/bitmap.hpp \
            NoLifeNx/file.hpp \
            NoLifeNx/nx.hpp \
            NoLifeNx/node.hpp

FORMS    += mainwindow.ui
LIBS += -lsfml-audio -lsfml-system -lmpg123 -llz4
