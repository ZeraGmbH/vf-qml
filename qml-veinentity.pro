#-------------------------------------------------
#
# Project created by QtCreator 2015-05-11T13:26:00
#
#-------------------------------------------------


#dependencies
VEIN_DEP_EVENT = 1
VEIN_DEP_COMP = 1
VEIN_DEP_HELPER = 1

HEADERS +=\
    qml-veinentity_global.h \
    entitycomponentmap.h \
    veinqml.h \
    veinqmlwrapper.h

TEMPLATE = lib
public_headers.files = $$HEADERS



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

exists( ../../vein-framework.pri ) {
  include(../../vein-framework.pri)
}
