#ifndef RADARTRACK_H
#define RADARTRACK_H

#include "qwt_point_polar.h"
#include <QColor>

/**
 * @brief 航迹信息结构体
 */
struct RadarTrackInfo {
    int index = -1; //批号
    double last_point_radius = 0; //末尾距离
    double last_point_azimuth = 0; //末尾方位
    bool is_end = false; //是否结束
};

class QwtPolarPlot;
class QwtPolarCurve;
class QwtSymbol;

class RadarTrackData;
class RadarTrackMarker;

class RadarTrack {
  public:
    RadarTrack();
    ~RadarTrack();

    //添加航迹点
    void AddTrackPoint(const RadarTrackInfo& info);
    //到指定点的最小距离
    double MinDistanceToPoint(const QwtPointPolar& polar);

    //设置航迹属性
    void SetMarked(bool is_marked);//标记
    void SetEnd(bool is_end);//结束

    //设置绘图区域
    void SetPlot(QwtPolarPlot* plot);

  private:
    //初始化
    void InitAll();
    void InitCurve();//初始化曲线
    void InitSymbol();//初始化标志
    void InitMarker();//初始化末尾标牌

    //设置颜色
    void SetColor(const QColor& color);

    //航迹曲线
    QwtPolarCurve* track_curve_ = nullptr;
    //标志点
    QwtSymbol* track_symbol_ = nullptr;
    //航迹数据
    RadarTrackData* track_data_ = nullptr;
    //航迹末尾标牌
    RadarTrackMarker* track_marker_ = nullptr;

    //未标记时颜色
    QColor color_ = Qt::yellow;
    //标记时颜色
    QColor color_marked_ = Qt::blue;
    //是否标记
    bool is_marked_ = false;

    //航迹属性
    RadarTrackInfo info_;

};

#endif // RADARTRACK_H
