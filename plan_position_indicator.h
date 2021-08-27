#ifndef PLANPOSITIONINDICATOR_H
#define PLANPOSITIONINDICATOR_H

#include "radar_points.h"
#include "radar_tracks.h"

#include "qwt_polar_plot.h"
#include "qwt_round_scale_draw.h"
#include "qwt_scale_draw.h"
#include "qwt_polar_grid.h"
#include "qwt_polar_panner.h"
#include "qwt_polar_magnifier.h"

#include <QTimer>

class PlanPositionIndicator: public QwtPolarPlot {
    Q_OBJECT

  public:
    explicit PlanPositionIndicator(QWidget* parent);

  public slots:
    //雷达点迹相关
    //添加点迹
    void AddPoint(quint16 cpi, double radius, double azimuth);
    //删除指定cpi周期的点迹
    void RemovePoints(quint16 cpi);
    //删除全部点迹
    void RemoveAllPoints();
    /**
     * 设置是否根据cpi自动删除，
     * true：interval有效，自动删除该间隔值之前的点。
     * false：interval无效。
     */
    void SetPointsAutoRemoveByCpi(bool enabled, quint16 interval = 20);
    //设置点迹容量
    void SetPointsCapacity(int points_capacity);
    //获取点迹容量
    int PointsCapacity();
    //设置点迹颜色
    void SetPointsColor(const QColor& points_color);
    //设置点迹尺寸
    void SetPointsSize(const QSize& points_size);

    //雷达航迹相关
    //添加航迹
    void AddTrackPoint(quint16 index, const RadarTrackInfo& info);
    //删除指定批号的航迹
    void RemoveTrack(quint16 index);

  protected:
    void mousePressEvent(QMouseEvent* event) override;

  private slots:
    void InitAll();//初始化
    void InitStyle();//初始化整体风格
    void InitScale();//初始化刻度
    void InitGrid();//初始化网格
    void InitPanner();//初始化平移器
    void InitZoomer();//初始化缩放器
    void InitRadarPoints();//初始化雷达点迹
    void InitRadarTracks();//初始化雷达航迹

    void InitRefreshTimer();//初始化刷新定时器

    void Refresh();//刷新

  private:
    int range_ = 30000;

    //网格
    QwtPolarGrid grid_;
    //平移器
    QwtPolarPanner* panner_ = nullptr;
    //缩放器
    QwtPolarMagnifier* zoomer_ = nullptr;

    //点迹
    RadarPoints* radar_points_ = nullptr;
    //航迹
    RadarTracks* radar_tracks_ = nullptr;

    //是否需要刷新
    bool is_need_refresh_ = false;
    //刷新定时器
    QTimer timer_refresh_;
};

class AzimuthScaleDraw: public QwtRoundScaleDraw {
  public:
    virtual QwtText label(double value) const;
};

class ScaleDraw: public QwtScaleDraw {
  public:
    virtual QwtText label(double value) const;
};
#endif // PLANPOSITIONINDICATOR_H
