#-------------------------------------------------
#
# Project created by QtCreator 2021-04-12T08:56:41
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = music-radar
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

CONFIG += c++11

# No debug output in release mode
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Set program version
VERSION = 1.0
DEFINES += VERSIONSTR=\\\"$${VERSION}\\\"


SOURCES += \
        about.cpp \
        elidedlabel.cpp \
        home.cpp \
        main.cpp \
        mainwindow.cpp \
        pulseutils.cpp \
        qaudiolevel.cpp \
        qaudiowaveform.cpp \
        recordingpage.cpp \
        remotepixmaplabel.cpp \
        rungaurd.cpp \
        songitem.cpp \
        utils.cpp \
        waitingspinnerwidget.cpp

HEADERS += \
        about.h \
        elidedlabel.h \
        home.h \
        mainwindow.h \
        pulseutils.h \
        qaudiolevel.h \
        qaudiowaveform.h \
        recordingpage.h \
        remotepixmaplabel.h \
        rungaurd.h \
        songitem.h \
        utils.h \
        waitingspinnerwidget.h

FORMS += \
        about.ui \
        home.ui \
        mainwindow.ui \
        qaudiowaveform.ui \
        recordingpage.ui \
        songitem.ui

# Default rules for deployment.
isEmpty(PREFIX){
 PREFIX = /usr
}

BINDIR  = $$PREFIX/bin
DATADIR = $$PREFIX/share

target.path = $$BINDIR

icon.files = icons/music-radar.png
icon.path = $$DATADIR/icons/hicolor/512x512/apps/

desktop.files = music-radar.desktop
desktop.path = $$DATADIR/applications/

INSTALLS += target icon desktop

RESOURCES += \
    icons.qrc

