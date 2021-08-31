#ifndef PLANPOSITIONINDICATOR_H
#define PLANPOSITIONINDICATOR_H

#include "qwt_polar_plot.h"
#include "qwt_round_scale_draw.h"
#include "qwt_scale_draw.h"
#include "radar_points.h"
#include "radar_track.h"

class QwtPolarGrid;
class QwtPolarPanner;
class QwtPolarMagnifier;

class QLabel;
class QMenu;

class PlanPositionIndicator : public QwtPolarPlot {
    Q_OBJECT
  public:
    explicit PlanPositionIndicator(QWidget* parent = nullptr);
    ~PlanPositionIndicator();

  public slots:
    //点迹相关
    //添加点迹
    void AddPoint(int cpi, double radius, double azimuth);
    //删除指定cpi点迹
    void RemovePoints(int cpi);
    //删除所有点迹
    void ClearPoints();
    /**
     * @brief 设置是否根据cpi间隔自动清除航迹
     * @param enable: 使能
     * @param per_cpi: 间隔多少cpi，使能关时无效
     */
    void SetAutoClearPointsByCpi(bool enable, int per_cpi = 20);
    //显示点迹
    void ShowPoints();
    //隐藏点迹
    void HidePoints();


    //航迹相关
    //添加航迹
    void AddTrackPoint(const RadarTrackInfo& info);
    //删除指定批号航迹
    void RemoveTrack(int index);
    //删除所有航迹
    void ClearTracks();
    /**
     * @brief 设置是否根据时间自动清除航迹
     * @param enbale:使能
     * @param timeout_msec:航迹超时时间(ms)，使能关时无效
     */
    void SetAutoClearTracksByTime(bool enable, int timeout_msec = 20000);
    //显示航迹
    void ShowTracks();
    //隐藏航迹
    void HideTracks();

    //手动清屏
    void ClearAll();


    //设置
    void SetRange(double range_meter);
    //设置点迹颜色
    void SetPointsColor(const QColor& color);
    //设置航迹颜色
    void SetTracksColor(const QColor& color);
    void SetTracksMarkedColor(const QColor& color);

  protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

  private slots:
    //初始化
    void InitAll();
    void InitStyle();//初始化整体风格
    void InitScale();//初始化刻度
    void InitGrid();//初始化网格
    void InitPanner();//初始化平移器
    void InitMagnifier();//初始化放大器
    void InitLabel();//初始化左上角标签
    void InitMenu();//初始化右键菜单
    void InitPoints();//初始化点迹
    void InitTimer();//初始化定时器

    //左上标签相关
    void UpdateLabel();

    //菜单相关
    void SetPointsStatus(bool is_show);//设置点迹显隐状态
    void SetTracksStatus(bool is_show);//设置航迹显隐状态

    //从哈希表中删除指定批号航迹
    void RemoveTrackFromHash(int index);
    //自动清航
    void AutoClearTracksByTime();
    //标记航迹
    void MarkTrack(const QwtPointPolar& polar);

    //刷新
    void Refresh();

  private:
    //整体样式
    double range_ = 30000; //量程:单位m
    QwtPolarGrid* grid_ = nullptr;//网格

    //控制
    QwtPolarPanner* panner_ = nullptr;//平移器
    QwtPolarMagnifier* magnifier_ = nullptr;//缩放器

    //左上标签
    QLabel* label_ = nullptr;
    double north_angle_ = 0;
    double mouse_radius_ = 0;
    double mouse_azimuth_ = 0;

    //右键菜单
    QMenu* menu_ = nullptr;

    //点迹
    RadarPoints* radar_points_ = nullptr;
    bool is_show_points_ = true;//是否显示点迹
    bool is_auto_clear_points_ = false;//是否自动清点
    int auto_clear_points_cpi_ = 20;//自动清点cpi间隔

    //航迹
    QHash<int, RadarTrack*> radar_tracks_;
    bool is_show_tracks_ = true;//是否显示航迹
    QColor tracks_color = Qt::yellow;
    QColor tracks_marked_color = Qt::blue;
    int max_marked_count_ = 5;
    int marked_count_ = 0;
    QTimer* timer_auto_clear_tracks_ = nullptr;//自动清航定时器

    //刷新
    bool is_need_refresh_ = false; //控制是否需要刷新
    QTimer* timer_refresh_ = nullptr;//刷新定时器

  signals:
    void CurrentMousePolar(double radius, double azimuth);
    void MarkedTrack(int index, bool is_marked);
    void RemovedTrack(int index);
};


#endif // PLANPOSITIONINDICATOR_H
