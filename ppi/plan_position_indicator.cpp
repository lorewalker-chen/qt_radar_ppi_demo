#include "plan_position_indicator.h"

#include "qwt_polar_grid.h"
#include "qwt_polar_panner.h"
#include "qwt_polar_magnifier.h"
#include "qwt_polar_canvas.h"

#include <QMenu>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
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
    if (timer_auto_clear_tracks_->isActive()) {
        timer_auto_clear_tracks_->stop();
    }
    timer_auto_clear_tracks_->deleteLater();

    if (timer_refresh_->isActive()) {
        timer_refresh_->stop();
    }
    timer_refresh_->deleteLater();

    delete radar_points_;

    delete grid_;
    delete panner_;
    delete magnifier_;

    label_->deleteLater();
    menu_->deleteLater();
}
//添加点迹
void PlanPositionIndicator::AddPoint(int cpi, double radius, double azimuth) {
    //根据cpi周期清点
    if (is_auto_clear_points_) {
        radar_points_->RemovePoints(cpi - auto_clear_points_cpi_);
    }
    radar_points_->AddPoint(cpi, QwtPointPolar(azimuth, radius));
    //如果不显示点迹
    if (!is_show_points_) {
        HidePoints();
    }
    //更改刷新标志
    is_need_refresh_ = true;
}
//删除指定cpi点迹
void PlanPositionIndicator::RemovePoints(int cpi) {
    radar_points_->RemovePoints(cpi);
    //更改刷新标志
    is_need_refresh_ = true;
}
//删除所有点迹
void PlanPositionIndicator::ClearPoints() {
    radar_points_->RemoveAllPoints();
    //更改刷新标志
    is_need_refresh_ = true;
}
//设置是否根据cpi间隔自动清除航迹
void PlanPositionIndicator::SetAutoClearPointsByCpi(bool enable, int per_cpi) {
    is_auto_clear_points_ = enable;
    auto_clear_points_cpi_ = per_cpi;
}
//显示点迹
void PlanPositionIndicator::ShowPoints() {
    radar_points_->ShowPoints();
    is_show_points_ = true;
}
//隐藏点迹
void PlanPositionIndicator::HidePoints() {
    radar_points_->HidePoints();
    is_show_points_ = false;
}
//添加航迹点
void PlanPositionIndicator::AddTrackPoint(const RadarTrackInfo& info) {
    //如果不存在该批号的航迹，新建
    if (!radar_tracks_.contains(info.index)) {
        RadarTrack* track = new RadarTrack(this);
        track->SetUnmarkedColor(tracks_color);
        track->SetMarkedColor(tracks_marked_color);
        radar_tracks_.insert(info.index, track);
    }
    //添加航迹点
    radar_tracks_.value(info.index)->AddTrackPoint(info);
    //如果不显示航迹
    if (!is_show_tracks_) {
        HideTracks();
    }
    //更改刷新标志
    is_need_refresh_ = true;
}
//删除指定批号航迹
void PlanPositionIndicator::RemoveTrack(int index) {
    if (!radar_tracks_.contains(index)) {
        return;
    }
    RemoveTrackFromHash(index);
    //更改刷新标志
    is_need_refresh_ = true;
}
//删除所有航迹
void PlanPositionIndicator::ClearTracks() {
    for (int index : radar_tracks_.keys()) {
        RemoveTrackFromHash(index);
    }
    //更改刷新标志
    is_need_refresh_ = true;
}
//设置是否根据时间自动清除航迹
void PlanPositionIndicator::SetAutoClearTracksByTime(bool enable, int timeout_msec) {
    if (enable) {
        timer_auto_clear_tracks_->start(timeout_msec / 2);
    } else if (timer_auto_clear_tracks_->isActive()) {
        timer_auto_clear_tracks_->stop();
    }
}
//显示航迹
void PlanPositionIndicator::ShowTracks() {
    for (int index : radar_tracks_.keys()) {
        radar_tracks_.value(index)->ShowTrack();
    }
    //更改显示航迹标志
    is_show_tracks_ = true;
    //更改刷新标志
    is_need_refresh_ = true;
}
//隐藏航迹
void PlanPositionIndicator::HideTracks() {
    for (int index : radar_tracks_.keys()) {
        radar_tracks_.value(index)->HideTrack();
    }
    //更改显示航迹标志
    is_show_tracks_ = false;
    //更改刷新标志
    is_need_refresh_ = true;
}
//手动清屏
void PlanPositionIndicator::ClearAll() {
    ClearPoints();
    ClearTracks();
}
//设置量程
void PlanPositionIndicator::SetRange(double range_meter) {
    //重新设置极径刻度
    setScale(QwtPolar::ScaleRadius, 0, range_meter);
    range_ = range_meter;
    //更改刷新标志
    is_need_refresh_ = true;
}
//设置点迹颜色
void PlanPositionIndicator::SetPointsColor(const QColor& color) {
    radar_points_->SetColor(color);
}
//设置航迹颜色
void PlanPositionIndicator::SetTracksColor(const QColor& color) {
    tracks_color = color;
}
//设置被标记航迹颜色
void PlanPositionIndicator::SetTracksMarkedColor(const QColor& color) {
    tracks_marked_color = color;
}
//事件过滤器
bool PlanPositionIndicator::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::HoverMove) {
        //悬停移动事件，获取当前鼠标位置
        QPoint point = canvas()->mapFromGlobal(QCursor::pos());
        //转化为极坐标
        QwtPointPolar polar = canvas()->invTransform(point);
        mouse_radius_ = polar.radius();
        mouse_azimuth_ = polar.azimuth();
        UpdateLabel();
    }
    return QFrame::eventFilter(obj, event);
}
//鼠标点击事件
void PlanPositionIndicator::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPoint point = canvas()->mapFromGlobal(QCursor::pos());
        //转化为极坐标
        QwtPointPolar polar = canvas()->invTransform(point);
        //标记航迹
        MarkTrack(polar);
    }
}
//键盘按键事件
void PlanPositionIndicator::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_F5) {
        ClearAll();
    }
}
//右键菜单事件
void PlanPositionIndicator::contextMenuEvent(QContextMenuEvent* event) {
    Q_UNUSED(event);
    menu_->exec(QCursor::pos());
}
//初始化
void PlanPositionIndicator::InitAll() {
    InitStyle();//初始化整体风格
    InitScale();//初始化刻度
    InitGrid();//初始化网格
    InitPanner();//初始化平移器
    InitMagnifier();//初始化放大器
    InitPoints();//初始化点迹
    InitLabel();//初始化左上角标签
    InitMenu();//初始化右键菜单
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
//初始化左上角标签
void PlanPositionIndicator::InitLabel() {
    label_ = new QLabel(this);
    label_->move(10, 10);
    label_->setStyleSheet("color:white");
    label_->setFont(QFont("Times New Romans", 10));
    UpdateLabel();
}
//初始化右键菜单
void PlanPositionIndicator::InitMenu() {
    menu_ = new QMenu;
    //点迹显隐
    QAction* show_points = new QAction("点迹", menu_);
    show_points->setCheckable(true);
    show_points->setChecked(true);
    connect(show_points, &QAction::toggled, this, &PlanPositionIndicator::SetPointsStatus);
    menu_->addAction(show_points);
    //航迹显隐
    QAction* show_tracks = new QAction("航迹", menu_);
    show_tracks->setCheckable(true);
    show_tracks->setChecked(true);
    connect(show_tracks, &QAction::toggled, this, &PlanPositionIndicator::SetTracksStatus);
    menu_->addAction(show_tracks);
    //间隔符
    menu_->addSeparator();
    //清空点迹
    menu_->addAction("清空点迹", this, &PlanPositionIndicator::ClearPoints);
    //清空航迹
    menu_->addAction("清空航迹", this, &PlanPositionIndicator::ClearTracks);
    //清屏
    menu_->addAction("清屏(F5)", this, &PlanPositionIndicator::ClearAll);
}
//初始化点迹
void PlanPositionIndicator::InitPoints() {
    radar_points_ = new RadarPoints(this);
}
//初始化定时器
void PlanPositionIndicator::InitTimer() {
    //自动清航定时器
    timer_auto_clear_tracks_ = new QTimer;
    connect(timer_auto_clear_tracks_, &QTimer::timeout, this, &PlanPositionIndicator::AutoClearTracksByTime);
    //刷新定时器
    timer_refresh_ = new QTimer;
    connect(timer_refresh_, &QTimer::timeout, this, &PlanPositionIndicator::Refresh);
    timer_refresh_->start(40);
}
//更新左上角标签
void PlanPositionIndicator::UpdateLabel() {
    QString str = QString("北向角：%0°\n距离：%1m\n方位：%2°\n")
                  .arg(QString::number(north_angle_, 'f', 2))
                  .arg(QString::number(mouse_radius_, 'f', 2))
                  .arg(QString::number(mouse_azimuth_, 'f', 2));
    label_->setText(str);
}
//设置点迹状态
void PlanPositionIndicator::SetPointsStatus(bool is_show) {
    if (is_show) {
        ShowPoints();
    } else {
        HidePoints();
    }
}
//设置航迹状态
void PlanPositionIndicator::SetTracksStatus(bool is_show) {
    if (is_show) {
        ShowTracks();
    } else {
        HideTracks();
    }
}
//从哈希表中删除指定批号航迹
void PlanPositionIndicator::RemoveTrackFromHash(int index) {
    delete radar_tracks_[index];
    radar_tracks_[index] = nullptr;
    radar_tracks_.remove(index);
    //发送信号通知该批号被删除
    emit RemovedTrack(index);
}
/**
 * @brief 根据时间自动清航
 * 每次调用删除已经被标记为结束的航迹，将其余航迹标记为结束
 * 下次调用前如果该航迹没有新增的点，则仍被标记为结束，将其删除
 */
void PlanPositionIndicator::AutoClearTracksByTime() {
    for (int index : radar_tracks_.keys()) {
        if (radar_tracks_.value(index)->IsEnd() && !radar_tracks_.value(index)->IsMarked()) {
            RemoveTrack(index);
        } else {
            radar_tracks_.value(index)->SetEnd(true);
        }
    }
}
//标记航迹
void PlanPositionIndicator::MarkTrack(const QwtPointPolar& polar) {
    double distance = 10;
    int index = -1;
    for (int key : radar_tracks_.keys()) {
        double temp_distance = radar_tracks_.value(key)->MinDistanceToPoint(polar);
        if (temp_distance < distance) {
            distance = temp_distance;
            index = key;
        }
    }
    //如果有附近的航迹
    if (-1 != index) {
        //该批航迹是否已经被标记
        bool is_markd = radar_tracks_.value(index)->IsMarked();
        if (!is_markd) {
            //如果还未标记，检查标记航迹数量是否到达最大数量
            if (marked_count_ >= max_marked_count_) {
                return;
            } else {
                marked_count_ += 1;
            }
        } else {
            marked_count_ -= 1;
        }
        radar_tracks_.value(index)->Mark();
        //发送改变标记的航迹批号，和是否标记
        emit MarkedTrack(index, !is_markd);
    }
}
//刷新
void PlanPositionIndicator::Refresh() {
    if (is_need_refresh_) {
        replot();
    }
}
