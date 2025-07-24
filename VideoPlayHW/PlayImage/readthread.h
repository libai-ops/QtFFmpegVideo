#ifndef READTHREAD_H
#define READTHREAD_H

#include <QObject>
#include <QTimer>
#include <QThread>

class ReadThread:public QThread
{
    Q_OBJECT
    enum
    {
        play,
        end
    };
public:
    ReadThread();
    ~ReadThread();

    void open(const QString strUrl); //打开视频
    void pause(bool bFlag);

protected:
    void run() override;


private:
    QString m_strUrl;
    bool    m_bPlay;  //播放控制
    bool    m_bPause; //暂停控制
};

#endif // READTHREAD_H
