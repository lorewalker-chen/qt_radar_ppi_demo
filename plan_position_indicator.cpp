#include "plan_position_indicator.h"

PlanPositionIndicator::PlanPositionIndicator(QWidget* parent): QwtPolarPlot(parent) {
    //根据传入区域调整大小
    resize(parent->width(), parent->height());
    //初始化
    InitAll();
}
//初始化
void PlanPositionIndicator::InitAll() {
    InitStyle();
    InitScale();
    InitGrid();
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
    //极径刻度，默认：0～30000m
    setScale(QwtPolar::ScaleRadius, 0, range_);
}
//初始化网格
void PlanPositionIndicator::InitGrid() {
    //线的画笔
    grid_.setPen(QPen(Qt::green, 1, Qt::SolidLine));
    //文字的画笔
    grid_.setAxisPen(QwtPolar::AxisAzimuth, QPen(Qt::green, 1));
    //字体
    grid_.setFont(QFont("Times New Romans", 10, QFont::Bold));
    //角度刻度绘制
    grid_.setAzimuthScaleDraw(new AzimuthScaleDraw);
    //距离刻度只显示右边
    grid_.showAxis(QwtPolar::AxisTop, false);
    grid_.showAxis(QwtPolar::AxisBottom, false);
    grid_.showAxis(QwtPolar::AxisLeft, false);
    grid_.showAxis(QwtPolar::AxisRight, true);
    //距离刻度绘制
    grid_.setScaleDraw(QwtPolar::AxisRight, new ScaleDraw);
    //依附到绘制区域
    grid_.attach(this);
}
//初始化平移器
void PlanPositionIndicator::InitPanner() {
    panner_ = new QwtPolarPanner(this->canvas());
    //开启可平移
    panner_->setEnabled(true);
}
//初始化缩放器
void PlanPositionIndicator::InitZoomer() {
    zoomer_ = new QwtPolarMagnifier(this->canvas());
    //开启可缩放
    zoomer_->setEnabled(true);
}
//角度刻度文字
QwtText AzimuthScaleDraw::label(double value) const {
    //将360改为0
    if (qFuzzyCompare(fmod(value, 360), 0.0)) {
        return QString("0");
    }
    return QwtRoundScaleDraw::label(value);
}
//距离刻度文字
QwtText ScaleDraw::label(double value) const {
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
