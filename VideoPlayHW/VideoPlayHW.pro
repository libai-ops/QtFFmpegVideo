QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


#  定义程序版本号
VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

TARGET = VideoPlayPlay
#CONFIG(debug, debug|release) {
win32:{
      DESTDIR = $$PWD/../bin
}
unix:{
     DESTDIR = $$PWD/../linux_bin
}
#}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    PlayImage/playimage.cpp \
    PlayImage/readthread.cpp \
    PlayImage/videodecode.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    PlayImage/playimage.h \
    PlayImage/readthread.h \
    PlayImage/videodecode.h \
    widget.h

FORMS += \
    widget.ui


# ffmpeg读取视频图像模块# 加载库，ffmpeg n5.1.2版本
#include(./VideoPlay/VideoPlay.pri)
win32:{
INCLUDEPATH += $$PWD/externfiles/ffmpeg/win/include
LIBS +=  -L$$PWD\externfiles\ffmpeg\win\lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale
}
unix:!macx{
LIBS += -L$$PWD\..\externfiles\ffmpeg\linux\lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale
INCLUDEPATH += $$PWD/../externfiles/ffmpeg/linux/include
}


DISTFILES += \
    VideoPlay/VideoPlay.pri
