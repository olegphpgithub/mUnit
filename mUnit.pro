#-------------------------------------------------
#
# Project created by QtCreator 2018-07-20T12:58:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mUnit
TEMPLATE = app


SOURCES += main.cpp\
    SpyWindow.cpp \
    Verifier.cpp \
        mainwindow.cpp \
    LaunchProcess.cpp \
    Launcher.cpp \
    ProcessUtil.cpp

HEADERS  += mainwindow.h \
    LaunchProcess.h \
    Launcher.h \
    SpyWindow.h \
    Verifier.h \
    ProcessUtil.h

FORMS    += mainwindow.ui

RC_FILE = mUnit.rc

win32 {
    LIBS += -lUser32
}
