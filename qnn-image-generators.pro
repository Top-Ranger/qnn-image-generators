#-------------------------------------------------
#
# Project created by QtCreator 2015-07-06T17:39:53
#
#-------------------------------------------------

QT       += core

TARGET = qnn-image-generators
TEMPLATE = lib

DEFINES += QNN_LIBRARY
VERSION = 0.0.1

INCLUDEPATH += src/ ../qnn/src

unix: LIBS += -L$$PWD/../qnn/ -lqnn
win32: LIBS += -L$$PWD/../qnn/ -lqnn0

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \ 
    src/network/imagedirectencodinggeneratornetwork.cpp

HEADERS += \ 
    src/network/imagedirectencodinggeneratornetwork.h

DESTDIR = $$PWD

OTHER_FILES += \
    LICENSE.LGPL3 \
    LICENSE.GPL3 \
    README
