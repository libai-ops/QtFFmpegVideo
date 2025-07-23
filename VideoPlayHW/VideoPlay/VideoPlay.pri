#---------------------------------------------------------------------------------------
# @功能：       ffmpeg读取视频图像模块
# @编译器：     Desktop Qt 6.5.3 MING 64bit（也支持其它编译器）
# @Qt IDE：
#
# @开发者     dyc
# @邮箱       libaiops@gmail.com
# @时间       2025-07-23 14:27:25
# @备注
#---------------------------------------------------------------------------------------

# 加载库，ffmpeg n5.1.2版本
win32{
LIBS += -L$$PWD\..\externfiles\ffmpeg\win\lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale
INCLUDEPATH += $$PWD/../externfiles/ffmpeg/win/include
}

unix:!macx{
LIBS += -L$$PWD\..\externfiles\ffmpeg\linux\lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale
INCLUDEPATH += $$PWD/../externfiles/ffmpeg/linux/include
}

# HEADERS += \
#     $$PWD/readthread.h \
#     $$PWD/videodecode.h

# SOURCES += \
#     $$PWD/readthread.cpp \
#     $$PWD/videodecode.cpp
