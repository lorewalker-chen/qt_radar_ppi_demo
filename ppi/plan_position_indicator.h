#ifndef PLANPOSITIONINDICATOR_H
#define PLANPOSITIONINDICATOR_H

#include "qwt_polar_plot.h"
#include "qwt_round_scale_draw.h"
#include "qwt_scale_draw.h"
#include "radar_track.h"

class QwtPolarGrid;
class QwtPolarPanner;
class QwtPolarMagnifier;

class PlanPositionIndicator : public QwtPolarPlot {
    Q_OBJECT
  public:
    explicit PlanPositionIndicator(QWidget* parent = nullptr);
    ~PlanPositionIndicator();

  public slots:
    //添加航迹
    void AddTrack(RadarTrackInfo info);
    //删除航迹
    void RemoveTrack(int index);
    //设置
    void SetRange(double range);

  protected:
    bool eventFilter(QObject* obj, QEvent* event);
    void mousePressEvent(QMouseEvent* event);

  private slots:
    //初始化
    void InitAll();
    void InitStyle();//初始化整体风格
    void InitScale();//初始化刻度
    void InitGrid();//初始化网格
    void InitPanner();//初始化平移器
    void InitMagnifier();//初始化放大器
    void InitTimer();//初始化定时器

    //刷新
    void Refresh();

  private:
    //整体样式
    double range_ = 30000; //量程
    QwtPolarGrid* grid_ = nullptr;//网格

    //控制
    QwtPolarPanner* panner_ = nullptr;//平移器
    QwtPolarMagnifier* magnifier_ = nullptr;//缩放器

    //航迹
    QHash<int, RadarTrack> radar_tracks_;

    //刷新
    bool is_need_refresh_ = false; //控制是否需要刷新
    QTimer* timer_refresh_ = nullptr;//刷新定时器

  signals:
    void CurrentMousePolar(double radius, double azimuth);
};


#endif // PLANPOSITIONINDICATOR_H
