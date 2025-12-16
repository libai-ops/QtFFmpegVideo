#ifndef VIDEODECODE_H
#define VIDEODECODE_H

#include <QString>
#include <QSize>
#include <qlist.h>

struct AVFormatContext;
struct AVCodecContext;
struct AVRational;
struct AVPacket;
struct AVFrame;
struct AVCodec;
struct SwsContext;
struct AVBufferRef;
class QImage;

class VideoDecode
{
public:
    VideoDecode();
    ~VideoDecode();

public:
    void initFFmpeg();
    void initHWDecoder(const AVCodec* codec);
    bool initObject();                              //初始化对象
    bool dataCopy();
    QImage read();
    void close();
    void clear();
    void free();
    void showError(int err);                      // 显示ffmpeg执行错误时的错误信息
    bool open(const QString& url);
    qreal rationalToDouble(AVRational* rational);

    bool isEnd();                                 // 是否读取完成
    const qint64& pts();                          // 获取当前帧显示时间
    const qint64& getTotalTimer();                //获取总时间
    QString timeToString(qint64 milliseconds);

private:
    AVFormatContext* m_formatContext = nullptr;   // 解封装上下文
    AVCodecContext*  m_codecContext  = nullptr;   // 解码器上下文
    SwsContext*      m_swsContext    = nullptr;   // 图像转换上下文
    AVPacket* m_packet = nullptr;                 // 数据包
    AVFrame*  m_frame  = nullptr;                 // 解码后的视频帧
    AVFrame*  m_frameHW = nullptr;                // 硬件解码后的视频帧
    int    m_videoIndex   = 0;                    // 视频流索引
    qint64 m_totalTime    = 0;                    // 视频总时长
    QString m_strTotalTimer = "";
    qint64 m_totalFrames  = 0;                    // 视频总帧数
    qint64 m_obtainFrames = 0;                    // 视频当前获取到的帧数
    qint64 m_pts          = 0;                    // 图像帧的显示时间
    qreal  m_frameRate    = 0;                    // 视频帧率
    QSize  m_size;                                // 视频分辨率大小
    char*  m_error = nullptr;                     // 保存异常信息
    bool   m_end = false;                         // 视频读取完成
    uchar* m_buffer = nullptr;

    QList<int> m_HWDeviceTypes;                   // 保存当前环境支持的硬件解码器
    AVBufferRef* hw_device_ctx = nullptr;         // 对数据缓冲区的引用
    bool   m_HWDecoder = false;                   // 记录是否使用硬件解码
};

#endif // VIDEODECODE_H
