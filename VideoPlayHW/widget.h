#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
 #include "./PlayImage/readthread.h"
#include "./PlayImage/playimage.h"
#include <QVariantAnimation>
#include <QPropertyAnimation>
#include <QPainter>
#include <QPainterPath>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_but_file_clicked();

    void on_but_open_clicked();

    void on_but_pause_clicked();

    void on_playState(ReadThread::PlayState state);

    void on_check_HW_clicked(bool checked);

    void slotMouseReleaseDouble(bool bIsPlay);

private:
    Ui::Widget *ui;

     PlayImage*  m_pPlayImage = nullptr;
     ReadThread* m_readThread = nullptr;
};


//小心心
class HeartWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(qreal pulseScale READ pulseScale WRITE setPulseScale NOTIFY pulseScaleChanged)

public:
    explicit HeartWidget(QWidget *parent = nullptr);

    qreal progress() const { return m_progress; }
    void setProgress(qreal p);

    qreal pulseScale() const { return m_pulseScale; }
    void setPulseScale(qreal s);

signals:
    void progressChanged(qreal);
    void pulseScaleChanged(qreal);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // 生成心形上给定 t(0..1) 的点（参数化）
    QPointF pointAt(qreal t) const;

    qreal m_progress = 0.0;     // 0..1，绘制进度
    qreal m_pulseScale = 1.0;   // 呼吸缩放
    QVariantAnimation *m_drawAnim = nullptr;
    QPropertyAnimation *m_pulseAnim = nullptr;

    QVector<QPointF> m_fullPoints; // 采样好的心形点（0..1 顺序）
    int m_samples = 600;           // 采样点数，越大越平滑
};


//加载页面
#include <QTimer>

class LoadingSpinnerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoadingSpinnerWidget(QWidget *parent = nullptr);

    void start();
    void stop();
    bool isRunning() const { return m_timer->isActive(); }

    void setText(const QString &txt) { m_text = txt; update(); }
    QString text() const { return m_text; }

    // 可调整视觉参数
    void setLineCount(int n) { m_lineCount = qMax(4, n); update(); }
    void setLineLength(int len) { m_lineLength = len; update(); }
    void setLineWidth(int w) { m_lineWidth = w; update(); }
    void setInterval(int ms) { m_timer->setInterval(ms); }

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override { return QSize(120, 120); }

private slots:
    void onTimeout();

private:
    QTimer *m_timer;
    int m_frame;            // 动画帧索引
    int m_lineCount;        // 菊花瓣数量
    int m_lineLength;       // 每瓣长度
    int m_lineWidth;        // 每瓣宽度
    QString m_text;         // 下方文字
};

#endif // WIDGET_H
