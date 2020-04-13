#-------------------------------------------------
#
# Project created by QtCreator 2016-04-28T15:51:29
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qgps
TEMPLATE = app


SOURCES += main.cpp\
    gps.cpp \
    serial.cpp \
    gpswindow.cpp \
    serialsetup.cpp

HEADERS  += \
    gps.h \
    serial.h \
    gpswindow.h \
    serialsetup.h

FORMS += \
    gpswindow.ui \
    serialsetup.ui
unix {
    target.path = /home/root
    INSTALLS += target
}
