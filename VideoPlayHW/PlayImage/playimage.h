#ifndef PLAYIMAGE_H
#define PLAYIMAGE_H

#include <QWidget>
#include <qmutex.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include "readthread.h"

class PlayImage : public QWidget
{
    Q_OBJECT
public:
    explicit PlayImage(QWidget *parent = nullptr);

    void updateImage(const QImage& image);
    void updatePixmap(const QPixmap& pixmap);

public slots:
    void on_playState(ReadThread::PlayState state);

signals:
    void sigMouseReleaseDouble(bool bIsPlay);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QPixmap m_pixmap;
    QImage  m_image;
    QMutex m_mutex;

    QLabel* m_pNoSignalText;

    //双击暂停播放
    bool m_bIsMouseDoubleStart = true;


};

#endif // PLAYIMAGE_H
