#include "radar_track.h"

#include "radar_track_data.h"
#include "radar_track_marker.h"

#include "qwt_polar_curve.h"
#include "qwt_symbol.h"

RadarTrack::RadarTrack() {
    //初始化
    InitAll();
}

RadarTrack::~RadarTrack() {
}
//添加航迹点
void RadarTrack::AddTrackPoint(const RadarTrackInfo& info) {
    //如果是第一个点，修改标牌文字
    if (-1 == info_.index) {
        track_marker_->SetText(QString::number(info.index));
    }
    //更新航迹属性
    info_ = info;
    QwtPointPolar polar = QwtPointPolar(info_.last_point_azimuth, info_.last_point_radius);
    //添加航迹点
    track_data_->AddTrackPoint(polar);
    //更新标牌位置
    track_marker_->setPosition(polar);
}
//到指定点的最小距离
double RadarTrack::MinDistanceToPoint(const QwtPointPolar& polar) {
    return track_data_->MinDistanceToPoint(polar);
}
//设置航迹是否标记
void RadarTrack::SetMarked(bool is_marked) {
    if (is_marked) {
        //变为标记颜色
        SetColor(color_marked_);
        //详细标牌文字
        QString str = QString("批号：%0\n距离：%1\n方位：%2")
                      .arg(info_.index)
                      .arg(info_.last_point_radius)
                      .arg(info_.last_point_azimuth);
        track_marker_->SetText(str);
    } else {
        //变为未标记颜色
        SetColor(color_);
        //标牌文字只显示批号
        track_marker_->SetText(QString::number(info_.index));
    }
    is_marked_ = is_marked;
}
//设置航迹是否结束
void RadarTrack::SetEnd(bool is_end) {
    info_.is_end = is_end;
}
//设置绘图区域
void RadarTrack::SetPlot(QwtPolarPlot* plot) {
    //曲线依附到绘图区域
    track_curve_->attach(plot);
    //末尾标牌依附到绘图区域
    track_marker_->attach(plot);
}
//初始化
void RadarTrack::InitAll() {
    InitSymbol();//初始化标志
    InitMarker();//初始化末尾标牌
    InitCurve();//初始化曲线
}
//初始化曲线
void RadarTrack::InitCurve() {
    track_curve_ = new QwtPolarCurve;
    //线条画笔，颜色宽度
    track_curve_->setPen(QPen(color_, 2));
    //画线
    track_curve_->setStyle(QwtPolarCurve::Lines);
    //设置标志
    track_curve_->setSymbol(track_symbol_);
    //设置数据
    track_data_ = new RadarTrackData;
    track_curve_->setData(track_data_);
}
//初始化标志
void RadarTrack::InitSymbol() {
    track_symbol_ = new QwtSymbol;
    //圆形
    track_symbol_->setStyle(QwtSymbol::Ellipse);
    //初始轮廓颜色
    track_symbol_->setPen(color_);
    //初始填充颜色
    track_symbol_->setBrush(QBrush(color_));
    //尺寸
    track_symbol_->setSize(QSize(4, 4));
}
//初始化末尾标牌
void RadarTrack::InitMarker() {
    track_marker_ = new RadarTrackMarker;
    //初始颜色
    track_marker_->SetColor(color_);
}
//设置颜色
void RadarTrack::SetColor(const QColor& color) {
    track_symbol_->setPen(color);
    track_symbol_->setBrush(QBrush(color));
    track_marker_->SetColor(color);
}
