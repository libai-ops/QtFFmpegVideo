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

private:
    char* m_error;
    QList<int> m_HWDeviceTypes;
};

#endif // VIDEODECODE_H
