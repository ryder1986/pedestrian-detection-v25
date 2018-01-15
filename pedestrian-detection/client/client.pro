#-------------------------------------------------
#
# Project created by QtCreator 2017-12-20T16:41:31
#
#-------------------------------------------------
include(../pedestrian-detection.pri)
include(../common/common.pri)
include(../common/client.pri)
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    client.cpp \
    searchwidget.cpp

HEADERS  += mainwindow.h \
    client.h \
    searchwidget.h
unix{
message(build $$TARGET on unix system)
#INSTALL_PATH="/root/repo-github/install-pedestrian-detection"
install_setting.path=$$OUT_PWD/install
install_setting.files=$$OUT_PWD/$$TARGET config.json run.sh
message(install ($$install_setting.files) in ($$install_setting.path))
INSTALLS+=install_setting
}else{


message(build $$TARGET on win32)
install_setting.path=$$OUT_PWD/install
    CONFIG(debug, debug|release){
        install_setting.files=$$OUT_PWD/debug/client.exe  config.json run-client.sh
    }else{
        install_setting.files=$$OUT_PWD/release/client.exe  config.json run-client.sh
    }
INSTALLS=install_setting
message(install ($$install_setting.files) in ($$install_setting.path))
}

FORMS += \
    form.ui \
    searchwidget.ui

