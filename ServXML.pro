QT -=   qui
QT +=   core\
        xml\
        sql\

CONFIG += c++11

TARGET = ServXML
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    myservice.cpp

include(common.pri)
include(qtservice.pri)
include(mainfunctions.pri)

HEADERS += \
    myservice.h

