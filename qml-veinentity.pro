#-------------------------------------------------
#
# Project created by QtCreator 2015-05-11T13:26:00
#
#-------------------------------------------------

TEMPLATE = lib

#dependencies
VEIN_DEP_EVENT = 1
VEIN_DEP_COMP = 1
VEIN_DEP_HELPER = 1

HEADERS +=\
    qml-veinentity_global.h \
    entitycomponentmap.h \
    veinqml.h \
    veinqmlwrapper.h

exists( ../../project-paths.pri ) {
  include(../../project-paths.pri)
}


QT       += qml quick network

QT       -= gui

TARGET = qml-veinentity

DEFINES += QMLVEINENTITY_LIBRARY

SOURCES += \
    entitycomponentmap.cpp \
    veinqml.cpp \
    veinqmlwrapper.cpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
