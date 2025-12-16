#include "videodecode.h"

#include <QDebug>
#include <QImage>
#include <QMutex>
#include <qdatetime.h>


extern "C" {        // 用C规则编译指定的代码
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/hwcontext.h"
#include <libavutil/pixfmt.h>
}


#define ERROR_LEN 1024  // 异常信息数组长度
#define PRINT_LOG 1


/*********************************** FFmpeg获取GPU硬件解码帧格式的回调函数 *****************************************/
static enum AVPixelFormat g_pixelFormat;
/**
 * @brief      回调函数，获取GPU硬件解码帧的格式
 * @param s
 * @param fmt
 * @return
 */

AVPixelFormat get_hw_format(AVCodecContext* s , const enum AVPixelFormat* fmt)
{
    const enum AVPixelFormat* p;
    for(p = fmt ; *p != -1 ; p++)
    {
        if(*p == g_pixelFormat)
        {
            return *p;
        }
    }
    qDebug() << "无法获取硬件表面个格式";
    return AV_PIX_FMT_NONE;
}


VideoDecode::VideoDecode()
    :m_error(NULL)
{
    m_HWDeviceTypes.clear();
    m_error = new char[ERROR_LEN];

    qDebug() << "FFmpeg version:" << av_version_info();

    //获取支持的硬解码器
    AVHWDeviceType avType = AV_HWDEVICE_TYPE_NONE;      // ffmpeg支持的硬件解码器
    QStringList strTypes;
    while((avType = av_hwdevice_iterate_types(avType)) != AV_HWDEVICE_TYPE_NONE) //遍历查找支持的设备类型
    {
        m_HWDeviceTypes.append(avType);
        const char* ctype = av_hwdevice_get_type_name(avType);
        if(ctype)
        {
            strTypes.append(QString(ctype));
        }
    }
    qDebug() << "支持的硬件解码器: " << strTypes;

    initFFmpeg();  // 5.1.2版本不需要调用了
}

VideoDecode::~VideoDecode()
{

}

void VideoDecode::initFFmpeg()
{
    static bool isFirst = true;
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if(isFirst)
    {
        //        av_register_all();         // 已经从源码中删除
        /**
         * 初始化网络库,用于打开网络流媒体，此函数仅用于解决旧GnuTLS或OpenSSL库的线程安全问题。
         * 一旦删除对旧GnuTLS和OpenSSL库的支持，此函数将被弃用，并且此函数不再有任何用途。
         */
        avformat_network_init();
        isFirst = false;
    }

}

void VideoDecode::initHWDecoder(const AVCodec *codec)
{
    if(!codec) return;
    for(int i = 0 ; ; i++)
    {
        const AVCodecHWConfig* config = avcodec_get_hw_config(codec , i);
        if(!config)
        {
            qDebug() << "打开硬件解码失败";
            return;
        }

        if(config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX) //判断是否为设备类型
        {
            for(auto i : m_HWDeviceTypes)
            {
                if(config->device_type == AVHWDeviceType(i))
                {
                    g_pixelFormat = config->pix_fmt;

                    //打开指定类型的设备，并为其创建AVHWDeviceContext
                    int ret = av_hwdevice_ctx_create(&hw_device_ctx , config->device_type , nullptr , nullptr , 0);
                    if(ret < 0)
                    {
                        showError(ret);
                        free();
                        return;
                    }

                    qDebug() << "打开硬件解码器: " << av_hwdevice_get_type_name(config->device_type);
                    m_codecContext->hw_device_ctx = av_buffer_ref(hw_device_ctx); //创建一个对AVBuffer的新引用
                    m_codecContext->get_format = get_hw_format; //// 由一些解码器调用，以选择将用于输出帧的像素格式
                    return;

                }
            }
        }
    }
}

void VideoDecode::showError(int err)
{
#if PRINT_LOG
    memset(m_error, 0, ERROR_LEN);        // 将数组置零
    av_strerror(err, m_error, ERROR_LEN);
    qWarning() << "DecodeVideo Error：" << m_error;
#else
    Q_UNUSED(err)
#endif
}

bool VideoDecode::open(const QString &url)
{
    qDebug() << "open url->>" << url;
    if(url.isNull()) return false;

    AVDictionary* dict = nullptr;
    av_dict_set(&dict , "rtsp_rransport" , "tcp" , 0); // 设置rtsp流使用tcp打开，如果打开失败错误信息为【Error number -135 occurred】可以切换（UDP、tcp、udp_multicast、http），比如vlc推流就需要使用udp打开
    av_dict_set(&dict , "max_delay" , "3" , 0);
    av_dict_set(&dict , "timeout" , "1000000" , 0);


    //打开输入流并返回解封装上下文
    int ret = avformat_open_input(&m_formatContext,               //返回解封装上下文
                                  url.toStdString().data(),      //视频地址
                                  nullptr,                       //如果非null,此参数强制使用特定的输入格式，自动选择解封装器(文件格式)
                                  &dict
                                  );

    if(dict)
    {
        av_dict_free(&dict);
    }

    if(ret < 0)
    {
        showError(ret);
        free();
        return false;
    }

    ret = avformat_find_stream_info(m_formatContext , nullptr);
    if(ret < 0)
    {
        showError(ret);
        free();
        return false;
    }

    m_totalTime = m_formatContext->duration / (AV_TIME_BASE / 1000); //计算视频总时长(毫秒)
    m_strTotalTimer   = timeToString(m_totalTime);
#if PRINT_LOG
    qDebug() << "视频总时长"<< m_totalTime;
#endif

    //// 通过AVMediaType枚举查询视频流ID（也可以通过遍历查找），最后一个参数无用
    m_videoIndex = av_find_best_stream(m_formatContext , AVMEDIA_TYPE_VIDEO , -1 , -1 , nullptr , 0);
    if(m_videoIndex < 0)
    {
        showError(m_videoIndex);
        free();
        return false;
    }

    //// 通过查询到的索引获取视频流
    AVStream* videoStream = m_formatContext->streams[m_videoIndex];

    // 获取视频图像分辨率（AVStream中的AVCodecContext在新版本中弃用，改为使用AVCodecParameters）
    m_size.setWidth(videoStream->codecpar->width);
    m_size.setHeight(videoStream->codecpar->height);
    m_frameRate = rationalToDouble(&videoStream->avg_frame_rate);//视频帧率

    // 通过解码器ID获取视频解码器（新版本返回值必须使用const）
    const AVCodec* codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    m_totalFrames = videoStream->nb_frames;

#if PRINT_LOG
    qDebug() << QString("分辨率：[w:%1,h:%2] 帧率：%3  总帧数：%4  解码器：%5")
                    .arg(m_size.width()).arg(m_size.height()).arg(m_frameRate).arg(m_totalFrames).arg(codec->name);
#endif


    //// 分配AVCodecContext并将其字段设置为默认值。
    m_codecContext = avcodec_alloc_context3(codec);
    if(!m_codecContext)
    {
        free();
        return false;
    }

    //// 使用视频流的codecpar为解码器上下文赋值
    ret = avcodec_parameters_to_context(m_codecContext , videoStream->codecpar);
    if(ret < 0)
    {
        showError(ret);
        free();
        return false;
    }

    m_codecContext->flags2 |= AV_CODEC_FLAG2_FAST;
    m_codecContext->thread_count = 8;

    if(m_HWDecoder)
    {
        initHWDecoder(codec);
    }

    // 初始化解码器上下文，如果之前avcodec_alloc_context3传入了解码器，这里设置NULL就可以
    ret = avcodec_open2(m_codecContext , nullptr, nullptr);
    if(ret < 0)
    {
        showError(ret);
        free();
        return false;
    }

    return initObject();
}

bool VideoDecode::initObject()
{
    // 分配AVPacket并将其字段设置为默认值。
    m_packet = av_packet_alloc();
    if(!m_packet)
    {
        free();
        return false;
    }

    // 分配AVFrame并将其字段设置为默认值。
    m_frame = av_frame_alloc();
    if(!m_frame)
    {
#if PRINT_LOG
        qWarning() << "av_frame_alloc() Error！";
#endif
        free();
        return false;
    }

    // 分配AVFrame并将其字段设置为默认值。
    m_frameHW = av_frame_alloc();
    if(!m_frameHW)
    {
#if PRINT_LOG
        qWarning() << "av_frame_alloc() Error！";
#endif
        free();
        return false;
    }

    // 由于传递时是浅拷贝，可能显示类还没处理完成，所以如果播放完成就释放可能会崩溃；
    if(m_buffer)
    {
        delete[] m_buffer;
        m_buffer = nullptr;
    }

     // 分配图像空间
    int size = av_image_get_buffer_size(AV_PIX_FMT_RGBA , m_size.width() , m_size.height() , 4);
    /**
     * 【注意：】这里可以多分配一些，否则如果只是安装size分配，大部分视频图像数据拷贝没有问题，
     *         但是少部分视频图像在使用sws_scale()拷贝时会超出数组长度，在使用使用msvc debug模式时delete[] m_buffer会报错（HEAP CORRUPTION DETECTED: after Normal block(#32215) at 0x000001AC442830370.CRT delected that the application wrote to memory after end of heap buffer）
     *         特别是这个视频流http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4
     */
    m_buffer = new uchar[size + 1000]; //   // 这里多分配1000个字节就基本不会出现拷贝超出的情况了，反正不缺这点内存
    m_end = false;
    return true;
}

/********************************* FFmpeg初始化硬件后将图像数据从GPU拷贝到CPU *************************************/
/**
 * @brief   硬件解码完成需要将数据从GPU复制到CPU
 * @return
 */
bool VideoDecode::dataCopy()
{
    if(m_frame->format != g_pixelFormat)
    {
        av_frame_unref(m_frame);
        return false;
    }

    int ret = av_hwframe_transfer_data(m_frameHW , m_frame , 0);
    if(ret < 0)
    {
        showError(ret);
        av_frame_unref(m_frame);
        return false;
    }
    av_frame_copy_props(m_frameHW , m_frame);// 仅将“metadata”字段从src复制到dst。
    return true;
}


/**
 * @brief    读取并返回视频图像
 * @return
 */
QImage VideoDecode::read()
{
    // 如果没有打开则返回
    if(!m_formatContext)
    {
        qDebug() << "跳帧...." << "QImage NULL";
        return QImage();
    }

    //读取下一帧数据
    int reatRet = av_read_frame(m_formatContext , m_packet);
    if(reatRet < 0)
    {
        avcodec_send_packet(m_codecContext , m_packet);
    }
    else
    {
        if(m_packet->stream_index == m_videoIndex) //// 如果是图像数据则进行解码
        {
            // 计算当前帧时间（毫秒）
#if 1       // 方法一：适用于所有场景，但是存在一定误差
            m_packet->pts = qRound64(m_packet->pts * (1000 * rationalToDouble(&m_formatContext->streams[m_videoIndex]->time_base)));
            m_packet->dts = qRound64(m_packet->dts * (1000 * rationalToDouble(&m_formatContext->streams[m_videoIndex]->time_base)));
#else       // 方法二：适用于播放本地视频文件，计算每一帧时间较准，但是由于网络视频流无法获取总帧数，所以无法适用
            m_obtainFrames++;
            m_packet->pts = qRound64(m_obtainFrames * (qreal(m_totalTime) / m_totalFrames));
#endif
            // 将读取到的原始数据包传入解码器
            int ret = avcodec_send_packet(m_codecContext , m_packet);
            if(ret < 0)
            {
                showError(ret);
            }
        }
    }

    av_packet_unref(m_packet);  // 释放数据包，引用计数-1，为0时释放空间

    if(!m_codecContext)
    {
        qDebug() << "m_codecContext isNull";
        m_end = true;
        return QImage();
    }

    if(!m_frame)
    {
        qDebug() << "m_frame isNull";
        m_end = true;
        return QImage();
    }


    int ret = avcodec_receive_frame(m_codecContext , m_frame);
    if(ret < 0)
    {
        av_frame_unref(m_frame);
        if(reatRet < 0)
        {
            m_end = true;
        }
        return QImage();
    }


    // 这样写是为了兼容软解码或者硬件解码打开失败情况
    AVFrame* m_frameTemp = m_frame;
    if(!m_frame->data[0])
    {
        m_frameTemp = m_frameHW;
        // 将解码后的数据从GPU拷贝到CPU
        if(!dataCopy())
        {
            return QImage();
        }
    }

    m_pts = m_frameTemp->pts;

    // 为什么图像转换上下文要放在这里初始化呢，是因为m_frame->format，如果使用硬件解码，解码出来的图像格式和m_codecContext->pix_fmt的图像格式不一样，就会导致无法转换为QImage
    if(!m_swsContext)
    {
        // 获取缓存的图像转换上下文。首先校验参数是否一致，如果校验不通过就释放资源；然后判断上下文是否存在，如果存在直接复用，如不存在进行分配、初始化操作
        m_swsContext = sws_getCachedContext(m_swsContext,
                                            m_frameTemp->width,                   // 输入图像的宽度
                                            m_frameTemp->height,                  // 输入图像的高度
                                            (AVPixelFormat)m_frameTemp->format,   // 输入图像的像素格式
                                            m_size.width(),                     // 输出图像的宽度
                                            m_size.height(),                    // 输出图像的高度
                                            AV_PIX_FMT_RGBA,                    // 输出图像的像素格式
                                            SWS_BILINEAR,                       // 选择缩放算法(只有当输入输出图像大小不同时有效),一般选择SWS_FAST_BILINEAR
                                            nullptr,                            // 输入图像的滤波器信息, 若不需要传NULL
                                            nullptr,                            // 输出图像的滤波器信息, 若不需要传NULL
                                            nullptr);                           // 特定缩放算法需要的参数(?)，默认为NULL
        if(!m_swsContext)
        {
#if PRINT_LOG
            qWarning() << "sws_getCachedContext() Error！";
#endif
            free();
            return QImage();
        }
    }

    // AVFrame转QImage
    uchar* data[]  = {m_buffer};
    int    lines[4];
    av_image_fill_linesizes(lines, AV_PIX_FMT_RGBA, m_frame->width);  // 使用像素格式pix_fmt和宽度填充图像的平面线条大小。
    ret = sws_scale(m_swsContext,             // 缩放上下文
                    m_frameTemp->data,            // 原图像数组
                    m_frameTemp->linesize,        // 包含源图像每个平面步幅的数组
                    0,                        // 开始位置
                    m_frameTemp->height,          // 行数
                    data,                     // 目标图像数组
                    lines);                   // 包含目标图像每个平面的步幅的数组

    // sws_context 输出改成 BGRA
    sws_getContext(
        m_frameTemp->width, m_frameTemp->height, (AVPixelFormat)m_frameTemp->format,
        m_size.width(), m_size.height(), AV_PIX_FMT_BGRA,
        SWS_BILINEAR,
        nullptr, nullptr, nullptr
        );

    qDebug() << "width: " << m_frameTemp->width << "height " << m_frameTemp->height;
    QImage image(m_buffer, m_frameTemp->width, m_frameTemp->height , lines[0] ,  QImage::Format_RGBA8888); //Format_RGBA8888
    av_frame_unref(m_frame);
    av_frame_unref(m_frameHW);

    QString strCurrentTimer = timeToString(m_pts);
    qDebug() << "myCrrentTimer" << strCurrentTimer << m_strTotalTimer;

    return image;
}

void VideoDecode::close()
{
    clear();
    free();

    m_totalTime     = 0;
    m_videoIndex    = 0;
    m_totalFrames   = 0;
    m_obtainFrames  = 0;
    m_pts           = 0;
    m_frameRate     = 0;
    m_size          = QSize(0, 0);
}

void VideoDecode::clear()
{
    // 因为avformat_flush不会刷新AVIOContext (s->pb)。如果有必要，在调用此函数之前调用avio_flush(s->pb)。
    if(m_formatContext && m_formatContext->pb)
    {
        avio_flush(m_formatContext->pb);
    }
    if(m_formatContext)
    {
        avformat_flush(m_formatContext);   // 清理读取缓冲
    }
}

void VideoDecode::free()
{
    // 释放上下文swsContext。
    if(m_swsContext)
    {
        sws_freeContext(m_swsContext);
        m_swsContext = nullptr;             // sws_freeContext不会把上下文置NULL
    }
    // 释放编解码器上下文和与之相关的所有内容，并将NULL写入提供的指针
    if(m_codecContext)
    {
        avcodec_free_context(&m_codecContext);
    }
    // 关闭并失败m_formatContext，并将指针置为null
    if(m_formatContext)
    {
        avformat_close_input(&m_formatContext);
    }
    if(hw_device_ctx)
    {
        av_buffer_unref(&hw_device_ctx);
    }
    if(m_packet)
    {
        av_packet_free(&m_packet);
    }
    if(m_frame)
    {
        av_frame_free(&m_frame);
    }
    if(m_frameHW)
    {
        av_frame_free(&m_frameHW);
    }
}


qreal VideoDecode::rationalToDouble(AVRational *rational)
{
    qreal frameRate = (rational->den == 0) ? 0 : (qreal(rational->num) / rational->den);
    return frameRate;
}

/**
 * @brief  视频是否读取完成
 * @return
 */
bool VideoDecode::isEnd()
{
    return m_end;
}


/**
 * @brief    返回当前帧图像播放时间
 * @return
 */
const qint64 &VideoDecode::pts()
{
    return m_pts;
}

const qint64 &VideoDecode::getTotalTimer()
{
    return m_totalTime;
}

QString VideoDecode::timeToString(qint64 milliseconds)
{
    if (milliseconds < 0) {
        return "00:00:00";
    }

    // 转换为秒
    qint64 totalSeconds = milliseconds / 1000;

    // 计算小时、分钟、秒
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}


