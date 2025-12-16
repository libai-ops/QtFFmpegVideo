#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>

extern "C" {        // 用C规则编译指定的代码
#include "libavcodec/avcodec.h"
}
// Q_DECLARE_METATYPE(AVFrame)  //注册结构体，否则无法通过信号传递AVFrame

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Qt+ffmpeg视频播放（软/硬解码 + OpenGL显示YUV/NV12）Demo V%1").arg(APP_VERSION));


    // 使用QOpenGLWindow绘制
    m_pPlayImage = new PlayImage;
#if USE_WINDOW
    ui->verticalLayout->addWidget(QWidget::createWindowContainer(m_pPlayImage));   // 这一步加载速度要比OpenGLWidget慢一点
#else
     ui->gridLayout->addWidget(m_pPlayImage);
#endif

    m_readThread = new ReadThread();
    connect(m_readThread, &ReadThread::updateImage, m_pPlayImage, &PlayImage::updateImage);
    connect(m_readThread, &ReadThread::playState, m_pPlayImage, &PlayImage::on_playState);
    connect(m_readThread, &ReadThread::playState, this, &Widget::on_playState);
    connect(m_pPlayImage, &PlayImage::sigMouseReleaseDouble , m_readThread, &ReadThread::slotMouseReleaseDouble);
    connect(m_pPlayImage, &PlayImage::sigMouseReleaseDouble , this , &Widget::slotMouseReleaseDouble);
}

Widget::~Widget()
{
    // 释放视频读取线程
     if(m_readThread)
     {
//         m_readThread->close();
         m_readThread->wait();
         delete m_readThread;
     }
    delete ui;
}

/**
 * @brief  获取本地视频路径
 */
void Widget::on_but_file_clicked()
{
    QString strName = QFileDialog::getOpenFileName(this, "选择播放视频~！", "/", "视频 (*.mp4 *.m4v *.mov *.avi *.flv);; 其它(*)");
    if(strName.isEmpty())
    {
        return;
    }
    ui->com_url->setCurrentText(strName);
}

/**
 * @brief  视频播放/停止
 */
void Widget::on_but_open_clicked()
{


#if 0 //小心心
    HeartWidget* w = new HeartWidget;
    w->resize(640, 480);
    w->show();
    w->raise();
#endif

#if 0   //加载过程
    QWidget* loading = new QWidget;
    loading->setFixedSize(1920,1080);
    loading->show();
    // 方式二：直接把它作为子 widget 并手动定位到父 widget 中心
    auto *spinner2 = new LoadingSpinnerWidget(loading);
    spinner2->setFixedSize(350,140);
    spinner2->start();

    // 手动居中（需要在父 widget resizeEvent 中重新定位）
    spinner2->move(
        (loading->width() - spinner2->width()) / 2,
        (loading->height() - spinner2->height()) / 2
        );
    spinner2->show();
#endif


    qDebug() << ui->but_open->text();
    if(ui->but_open->text() == "开始播放")
    {
        m_readThread->open(ui->com_url->currentText());
    }
    else
    {
        m_readThread->close();
    }
}

/**
 * @brief 视频暂停/继续
 */
void Widget::on_but_pause_clicked()
{
    if(ui->but_pause->text() == "暂停")
    {
        m_readThread->pause(true);
        ui->but_pause->setText("继续");
    }
    else
    {
        m_readThread->pause(false);
        ui->but_pause->setText("暂停");
    }
}

/**
 * @brief        根据视频播放状态切换界面设置
 * @param state
 */
void Widget::on_playState(ReadThread::PlayState state)
{
    if(state == ReadThread::play)
    {
        this->setWindowTitle(QString("正在播放：%1").arg(m_readThread->url()));
        ui->but_open->setText("停止播放");
    }
    else
    {
        ui->but_open->setText("开始播放");
        ui->but_pause->setText("暂停");
        this->setWindowTitle(QString("Qt+ffmpeg视频播放（软/硬解码 + OpenGL显示YUV/NV12）Demo V%1").arg(APP_VERSION));
    }
}

void Widget::on_check_HW_clicked(bool checked)
{
    // m_readThread->setHWDecoder(checked);
}

void Widget::slotMouseReleaseDouble(bool bIsPlay)
{
    if(bIsPlay)
    {
        m_readThread->pause(true);
        ui->but_pause->setText("继续");
    }
    else
    {
        m_readThread->pause(false);
        ui->but_pause->setText("暂停");
    }
}


//小心心================================================start
HeartWidget::HeartWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);

    // 预先采样心形点（按参数 t 0..1）
    m_fullPoints.reserve(m_samples);
    for (int i = 0; i < m_samples; ++i) {
        qreal t = qreal(i) / qreal(m_samples - 1);
        m_fullPoints.append(pointAt(t));
    }

    // 绘制进度动画（循环，从0到1再到0）
    m_drawAnim = new QVariantAnimation(this);
    m_drawAnim->setDuration(7000);
    m_drawAnim->setStartValue(0.0);
    m_drawAnim->setEndValue(1.0);
    m_drawAnim->setEasingCurve(QEasingCurve::InOutCubic);
    m_drawAnim->setLoopCount(1);
    connect(m_drawAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v){
        setProgress(v.toReal());
    });
    m_drawAnim->start();

    // 呼吸动画（小范围缩放）
    m_pulseAnim = new QPropertyAnimation(this, "pulseScale", this);
    m_pulseAnim->setDuration(900);
    m_pulseAnim->setStartValue(0.94);
    m_pulseAnim->setEndValue(1.06);
    m_pulseAnim->setEasingCurve(QEasingCurve::InOutSine);
    m_pulseAnim->setLoopCount(-1);
    // m_pulseAnim->setDirection(QAbstractAnimation::Alternate);
    m_pulseAnim->start();
}

void HeartWidget::setProgress(qreal p)
{
    if (p < 0) p = 0;
    if (p > 1) p = 1;
    if (!qFuzzyCompare(m_progress + 1, p + 1)) {
        m_progress = p;
        emit progressChanged(m_progress);
        update();
    }
}

void HeartWidget::setPulseScale(qreal s)
{
    if (!qFuzzyCompare(m_pulseScale + 1, s + 1)) {
        m_pulseScale = s;
        emit pulseScaleChanged(m_pulseScale);
        update();
    }
}

QPointF HeartWidget::pointAt(qreal t) const
{
    // 使用参数方程绘制一个标准心形，t in [0,1] -> theta in [0, 2*pi]
    qreal theta = t * M_PI * 2.0;
    // 常见心形参数： x = 16 sin^3(t); y = 13 cos t - 5 cos(2t) - 2 cos(3t) - cos(4t)
    qreal x = 16.0 * qPow(qSin(theta), 3.0);
    qreal y = 13.0 * qCos(theta) - 5.0 * qCos(2.0 * theta) - 2.0 * qCos(3.0 * theta) - qCos(4.0 * theta);

    // 这里返回的是未经缩放的点，后在 paintEvent 中按控件大小映射到像素坐标
    return QPointF(x, -y); // 反转 y 轴使其向上生长
}

void HeartWidget::resizeEvent(QResizeEvent * /*event*/)
{
    // nothing for now; points are parameter-based, we'll map in paint
}

void HeartWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QRectF r = rect().adjusted(10, 10, -10, -10);
    // 计算缩放因子，把心形整体放入 r 中
    // 心形参数的大致范围： x in [-16,16], y in [-? , ?], 大约高 22 左右
    qreal shapeW = 32.0;
    qreal shapeH = 26.0;
    qreal scale = qMin(r.width() / shapeW, r.height() / shapeH);
    scale *= 0.9; // 留边
    scale *= m_pulseScale; // 呼吸缩放

    QPointF center = r.center();

    // 构造绘制到目前为止的点序列（按 m_progress）
    int drawCount = qMax(2, int(m_fullPoints.size() * m_progress));
    QPainterPath path;
    for (int i = 0; i < drawCount; ++i) {
        QPointF pt = m_fullPoints[i];
        QPointF mapped = QPointF(pt.x() * scale + center.x(), pt.y() * scale + center.y());
        if (i == 0) path.moveTo(mapped);
        else path.lineTo(mapped);
    }

    // 背景渐变
    {
        QLinearGradient lg(r.topLeft(), r.bottomRight());
        lg.setColorAt(0.0, QColor(12, 12, 30));
        lg.setColorAt(1.0, QColor(20, 0, 40));
        p.fillRect(r, lg);
    }

    // 画一个淡淡的 glow（两次路径：一遍大的透明模糊，一遍正常）
    if (!path.isEmpty()) {
        // 大的 glow
        QPen glowPen(QColor(255, 80, 120, 50));
        glowPen.setWidthF(40.0 * (0.5 + m_progress * 0.5) * m_pulseScale);
        glowPen.setCapStyle(Qt::RoundCap);
        glowPen.setJoinStyle(Qt::RoundJoin);
        p.setPen(glowPen);
        p.drawPath(path);

        // 中间的渐变描边
        QPen pen;
        pen.setWidthF(6.0 * m_pulseScale);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);

        // 使用渐变笔触
        QLinearGradient strokeGrad(0, 0, width(), height());
        strokeGrad.setColorAt(0.0, QColor(255, 160, 190));
        strokeGrad.setColorAt(0.5, QColor(255, 80, 120));
        strokeGrad.setColorAt(1.0, QColor(180, 30, 120));
        pen.setBrush(QBrush(strokeGrad));
        p.setPen(pen);
        p.drawPath(path);

        // 填充（进度接近结束时填充心形）
        if (m_progress > 0.98) {
            QPainterPath fullPath;
            for (int i = 0; i < m_fullPoints.size(); ++i) {
                QPointF pt = m_fullPoints[i];
                QPointF mapped = QPointF(pt.x() * scale + center.x(), pt.y() * scale + center.y());
                if (i == 0) fullPath.moveTo(mapped);
                else fullPath.lineTo(mapped);
            }
            fullPath.closeSubpath();
            QLinearGradient fillGrad(center.x(), center.y() - shapeH*scale/2, center.x(), center.y() + shapeH*scale/2);
            fillGrad.setColorAt(0.0, QColor(255, 120, 170, 200));
            fillGrad.setColorAt(1.0, QColor(200, 40, 120, 200));
            p.fillPath(fullPath, fillGrad);
        }

        // 绘制在当前进度末端的流光小点
        if (drawCount > 1) {
            QPointF lastPt = m_fullPoints[qMin(drawCount - 1, m_fullPoints.size() - 1)];
            QPointF mappedLast(lastPt.x() * scale + center.x(), lastPt.y() * scale + center.y());

            // 发光外圈
            QColor glowCol(255, 200, 220);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(glowCol.red(), glowCol.green(), glowCol.blue(), 180));
            p.drawEllipse(mappedLast, 10.0 * m_pulseScale, 10.0 * m_pulseScale);

            // 核心亮点
            p.setBrush(QColor(255, 255, 255));
            p.drawEllipse(mappedLast, 4.0 * m_pulseScale, 4.0 * m_pulseScale);
        }
    }

    // optional: 在下方显示进度百分比（调试用）
    p.setPen(QColor(200,200,200,120));
    p.setFont(QFont("Arial", 10));
    p.drawText(10, height()-10, QString("progress: %1%").arg(int(m_progress*100)));
}
//小心心================================================end

//加载页面==============================================start
#include <QPainter>
#include <QPaintEvent>
#include <QtMath>

LoadingSpinnerWidget::LoadingSpinnerWidget(QWidget *parent)
    : QWidget(parent),
    m_timer(new QTimer(this)),
    m_frame(0),
    m_lineCount(12),
    m_lineLength(16),
    m_lineWidth(4),
    m_text(QStringLiteral("录屏结束，文件正在上传中...,请稍后"))
{
    // 推荐的动画间隔，可调整
    m_timer->setInterval(80);
    connect(m_timer, &QTimer::timeout, this, &LoadingSpinnerWidget::onTimeout);

    // 透明背景（如果需要可以改）
    setAttribute(Qt::WA_TranslucentBackground, true);

    // 鼠标穿透（如果你不想阻塞父界面事件）
    // setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

void LoadingSpinnerWidget::start()
{
    if (!m_timer->isActive()) {
        m_timer->start();
    }
}

void LoadingSpinnerWidget::stop()
{
    if (m_timer->isActive()) {
        m_timer->stop();
        m_frame = 0;
        update();
    }
}

void LoadingSpinnerWidget::onTimeout()
{
    m_frame = (m_frame + 1) % m_lineCount;
    update(); // 触发重绘
}

void LoadingSpinnerWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int w = width();
    const int h = height();

    // ========== 菊花部分 ==========
    int radius   = qMin(w, h) / 6;
    int lineLen  = m_lineLength;
    int lineW    = m_lineWidth;

    QPoint center(w/2, h/3); // 菊花位置：控件上方 1/3 处

    p.translate(center);
    const qreal step = 360.0 / m_lineCount;

    for (int i = 0; i < m_lineCount; ++i) {
        p.save();
        p.rotate(i * step);

        int idx = (i + m_frame) % m_lineCount;
        int alpha = 40 + (215 * (m_lineCount - 1 - idx)) / (m_lineCount - 1);

        QColor col(40, 180, 100);
        col.setAlpha(alpha);
        p.setPen(Qt::NoPen);
        p.setBrush(col);

        QRectF rect(radius - lineLen, -lineW/2.0, lineLen, lineW);
        p.drawRoundedRect(rect, lineW/2.0, lineW/2.0);

        p.restore();
    }

    // ========== 文字部分 ==========
    p.resetTransform();
    p.setPen(Qt::black);
    QFont font;
    font.setPixelSize(18);  // 调大一点，比如 24px
    p.setFont(font);


    int textTop = center.y() + radius + 15; // 菊花下方 15px
    QRect textRect(10, textTop, w - 20, h - textTop - 10);

    // 一行显示，不换行
    p.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextSingleLine, m_text);
}
//加载页面==============================================end
