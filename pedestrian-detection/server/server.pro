#-------------------------------------------------
#
# Project created by QtCreator 2017-12-20T16:43:49
#
#-------------------------------------------------
include(../pedestrian-detection.pri)
include(../common/common.pri)
include(../common/server.pri)
QT       += core network
QT       -= gui
TARGET = server
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
CONFIG +=c++11

LIBS+=-L$$ALGPATH/lib -lAlg



HEADERS += \
    server.h \
    camera.h \
    processeddatasender.h
SOURCES += main.cpp \
    server.cpp \
    camera.cpp \
    processeddatasender.cpp
unix{
message(build $$OUT_PWD/$$TARGET on unix system)
#INSTALL_PATH="/root/repo-github/install-pedestrian-detection"
install_setting.path=$$OUT_PWD/install
install_setting.files=$$OUT_PWD/$$TARGET config.json run.sh hogcascade_pedestrians.xml
message(install ($$install_setting.files) in ($$install_setting.path))
INSTALLS+=install_setting
}else{

message(build $$TARGET on win32)
install_setting.path=$$OUT_PWD/install
    CONFIG(debug, debug|release){
        install_setting.files=$$OUT_PWD/debug/server.exe  config.json run-server.sh hogcascade_pedestrians.xml
    }else{
        install_setting.files=$$OUT_PWD/release/server.exe  config.json run-server.sh hogcascade_pedestrians.xml
    }
INSTALLS=install_setting
message(install ($$install_setting.files) in ($$install_setting.path))
}
