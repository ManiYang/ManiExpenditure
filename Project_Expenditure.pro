#-------------------------------------------------
#
# Project created by QtCreator 2018-02-07T07:54:24
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Project_Expenditure
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
        main.cpp \
        mainwindow.cpp \
    utility_DOM.cpp \
    dialog_edit_item.cpp \
    item_data.cpp

HEADERS += \
        mainwindow.h \
    utility_DOM.h \
    dialog_edit_item.h \
    item_data.h \
    utility_math.h

FORMS += \
        mainwindow.ui \
    dialog_edit_item.ui
