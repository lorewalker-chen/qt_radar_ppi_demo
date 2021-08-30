#include "radar_track_data.h"

RadarTrackData::RadarTrackData() {
    data_all_.clear();
}
//添加点
void RadarTrackData::AddTrackPoint(const QwtPointPolar& polar) {
    data_all_.append(polar);
}
//离指定点的最小距离
double RadarTrackData::MinDistanceToPoint(const QwtPointPolar& polar) {
    double distance = 65535;
    QPointF point = polar.toPoint();
    for (QwtPointPolar polar : data_all_) {
        QPointF temp_p = polar.toPoint();
        double temp_d = sqrtf(powf((temp_p.x() - point.x()), 2) + powf((temp_p.y() - point.y()), 2));
        if (temp_d < distance) {
            distance = temp_d;
        }
    }
    return distance;
}
//返回总点数
size_t RadarTrackData::size() const {
    return data_all_.size();
}
//采样，返回从0~(size-1)的每个点
QwtPointPolar RadarTrackData::sample(size_t i) const {
    return data_all_.at(i);
}
//返回绘图区域
QRectF RadarTrackData::boundingRect() const {
    return qwtBoundingRect(*this);
}
