QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


#  定义程序版本号
VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    PlayImage/playimage.cpp \
    PlayImage/readthread.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    PlayImage/playimage.h \
    PlayImage/readthread.h \
    widget.h

FORMS += \
    widget.ui


# ffmpeg读取视频图像模块
include(./VideoPlay/VideoPlay.pri)

DISTFILES += \
    VideoPlay/VideoPlay.pri
