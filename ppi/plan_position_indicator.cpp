#include "plan_position_indicator.h"

#include "qwt_polar_grid.h"
#include "qwt_polar_panner.h"
#include "qwt_polar_magnifier.h"
#include "qwt_polar_canvas.h"

#include <QEvent>
#include <QMouseEvent>
#include <QTimer>

/**
 * @brief 自定义角度刻度文字
 */
class AzimuthScaleDraw: public QwtRoundScaleDraw {
  public:
    virtual QwtText label(double value) const {
        //将360改为0
        if (qFuzzyCompare(fmod(value, 360), 0.0)) {
            return QString("0");
        }
        return QwtRoundScaleDraw::label(value);
    }
};

/**
 * @brief 自定义距离刻度文字
 */
class ScaleDraw: public QwtScaleDraw {
  public:
    virtual QwtText label(double value) const {
        //不绘制中心的0
        if (value == 0) {
            return QString("");
        }
        //大于1000的加上单位km
        if (value > 1000) {
            QString temp = QString("%1").arg(value / 1000) + "km";
            return temp;
        }
        //其余加上单位m
        return QString("%1m").arg(value);
    }
};

/**
 * @brief 雷达平面方位指示器类
 */
PlanPositionIndicator::PlanPositionIndicator(QWidget* parent) : QwtPolarPlot(parent) {
    //适应父对象尺寸
    resize(parent->width(), parent->height());
    //初始化
    InitAll();
}

PlanPositionIndicator::~PlanPositionIndicator() {
    delete grid_;
    delete panner_;
    delete magnifier_;
}
//添加航迹
void PlanPositionIndicator::AddTrack(RadarTrackInfo info) {
    if (!radar_tracks_.contains(info.index)) {
        //如果没有该批号的航迹，新建一批
        RadarTrack track;
        track.SetPlot(this);
        track.AddTrackPoint(info);
        radar_tracks_.insert(info.index, track);
    } else {
        //如果已经有该批号的航迹，直接添加
        radar_tracks_[info.index].AddTrackPoint(info);
    }
    //刷新标志
    is_need_refresh_ = true;
}
//删除航迹
void PlanPositionIndicator::RemoveTrack(int index) {
    if (radar_tracks_.contains(index)) {
        radar_tracks_.remove(index);
    }
}
//设置量程
void PlanPositionIndicator::SetRange(double range) {
    //重新设置极径刻度
    setScale(QwtPolar::ScaleRadius, 0, range);
}
//事件过滤器
bool PlanPositionIndicator::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::HoverMove) {
        //悬停移动事件，获取当前鼠标位置
        QPoint point = canvas()->mapFromGlobal(QCursor::pos());
        //转化为极坐标
        QwtPointPolar polar = canvas()->invTransform(point);
        emit CurrentMousePolar(polar.radius(), polar.azimuth());
    }
    return QFrame::eventFilter(obj, event);
}
//鼠标点击事件
void PlanPositionIndicator::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        //获取当前鼠标位置
        QPoint point = canvas()->mapFromGlobal(QCursor::pos());
        //转化为极坐标
        QwtPointPolar polar = canvas()->invTransform(point);
    }
}
//初始化
void PlanPositionIndicator::InitAll() {
    InitStyle();//初始化整体风格
    InitScale();//初始化刻度
    InitGrid();//初始化网格
    InitPanner();//初始化平移器
    InitMagnifier();//初始化放大器
    InitTimer();//初始化定时器

    //开启悬停事件
    setAttribute(Qt::WA_Hover, true);
    //安装事件过滤器
    installEventFilter(this);
}
//初始化整体风格
void PlanPositionIndicator::InitStyle() {
    //背景色，黑色，整体填充
    setPlotBackground(QBrush(Qt::black, Qt::SolidPattern));
}
//初始化刻度
void PlanPositionIndicator::InitScale() {
    //极角起始，默认：90
    setAzimuthOrigin(90 * M_PI / 180.0);
    //极角刻度，默认：0～360，间隔30
    setScale(QwtPolar::ScaleAzimuth, 360.00, 0.00, 30.0);
    //极径刻度
    setScale(QwtPolar::ScaleRadius, 0, range_);
}
//初始化网格
void PlanPositionIndicator::InitGrid() {
    grid_ = new QwtPolarGrid;
    //线的画笔
    grid_->setPen(QPen(Qt::green, 1, Qt::SolidLine));
    //文字的画笔
    grid_->setAxisPen(QwtPolar::AxisAzimuth, QPen(Qt::green, 1));
    //字体
    grid_->setFont(QFont("Times New Romans", 10, QFont::Bold));
    //角度刻度绘制
    grid_->setAzimuthScaleDraw(new AzimuthScaleDraw);
    //距离刻度只显示右边
    grid_->showAxis(QwtPolar::AxisTop, false);
    grid_->showAxis(QwtPolar::AxisBottom, false);
    grid_->showAxis(QwtPolar::AxisLeft, false);
    grid_->showAxis(QwtPolar::AxisRight, true);
    //距离刻度绘制
    grid_->setScaleDraw(QwtPolar::AxisRight, new ScaleDraw);
    //依附到绘制区域
    grid_->attach(this);
}
//初始化平移器
void PlanPositionIndicator::InitPanner() {
    panner_ = new QwtPolarPanner(this->canvas());
    //开启可平移
    panner_->setEnabled(true);
}
//初始化放大器
void PlanPositionIndicator::InitMagnifier() {
    magnifier_ = new QwtPolarMagnifier(this->canvas());
    //开启可放大
    magnifier_->setEnabled(true);
}
//初始化定时器
void PlanPositionIndicator::InitTimer() {
    timer_refresh_ = new QTimer;
    connect(timer_refresh_, &QTimer::timeout, this, &PlanPositionIndicator::Refresh);
    timer_refresh_->start(40);
}
//刷新
void PlanPositionIndicator::Refresh() {
    if (is_need_refresh_) {
        replot();
    }
}
