#include "readthread.h"

ReadThread::ReadThread()
{

}

ReadThread::~ReadThread()
{

}

/**
 * @brief      传入播放的视频地址并开启线程
 * @param url
 */
void ReadThread::open(const QString strUrl)
{
    if(!this->isRunning() && strUrl.isEmpty())
    {
        m_strUrl = strUrl;
        emit this->start();
    }
}

void ReadThread::pause(bool bFlag)
{
    m_bPause = bFlag;
}

void ReadThread::run()
{

}
