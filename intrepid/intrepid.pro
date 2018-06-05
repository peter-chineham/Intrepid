#-------------------------------------------------
#
# Project created by QtCreator 2018-06-02T12:18:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = intrepid
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    arc.cpp \
    canvas.cpp \
    Dictionary.cpp \
    Edge.cpp \
    equipment.cpp \
    linetype.cpp \
    main.cpp \
    mainwindow.cpp \
    mentmux.cpp \
    mentor.cpp \
    Network.cpp \
    Node.cpp \
    Parm.cpp \
    Reader2.cpp \
    Requirements.cpp \
    Utils.cpp \
    world.cpp

HEADERS += \
    arc.h \
    canvas.h \
    Dictionary.h \
    Edge.h \
    equipment.h \
    linetype.h \
    mainwindow.h \
    mentmux.h \
    mentor.h \
    Network.h \
    Node.h \
    Parm.h \
    Reader2.h \
    Requirements.h \
    Utils.h \
    world.h

FORMS += \
        mainwindow.ui
