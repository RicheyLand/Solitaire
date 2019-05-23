#-------------------------------------------------
#
# Project created by QtCreator 2019-05-04T19:58:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = solitaire
TEMPLATE = app


SOURCES += main.cpp\
    core.cpp \
    klondike.cpp

HEADERS  += \
    core.h \
    klondike.h

FORMS += \
    klondike.ui

RESOURCES += \
    resources.qrc

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -std=c++11 -Wall -Wextra -pedantic -O3

DISTFILES +=
