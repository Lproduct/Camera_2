#-------------------------------------------------
#
# Project created by QtCreator 2015-07-09T17:04:29
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Camera_2
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

HEADERS += \
    Common/exampleHelper.h \
    mvIMPACT_CPP/mvIMPACT_acquire.h \
    mvDisplay/Include/mvIMPACT_acquire_display.h \
    mvDeviceManager/Include/mvDeviceManager.h \
    DriverBase/Include/mvDriverBaseEnums.h \
    mvPropHandling/Include/mvPropHandlingDatatypes.h

win32: LIBS += -L$$PWD/lib/ -lmvDeviceManager

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/mvDeviceManager.lib

win32: LIBS += -L$$PWD/lib/ -lmvDisplay

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/mvDisplay.lib
