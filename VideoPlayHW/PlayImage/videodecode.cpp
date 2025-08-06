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
}


#define ERROR_LEN 1024  // 异常信息数组长度
#define PRINT_LOG 1


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
}

VideoDecode::~VideoDecode()
{

}
