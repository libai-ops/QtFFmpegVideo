#ifndef PLAYIMAGE_H
#define PLAYIMAGE_H

#include <QWidget>
#include <qmutex.h>
#include <QLabel>
#include <QVBoxLayout>
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

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_pixmap;
    QMutex m_mutex;

    QLabel* m_pNoSignalText;


};

#endif // PLAYIMAGE_H
