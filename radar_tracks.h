#ifndef RADARTRACKS_H
#define RADARTRACKS_H

#include "qwt_series_data.h"

#include "qwt_polar_marker.h"

#include "qwt_polar_curve.h"

struct RadarTrackInfo {
    int index = -1; //批号
    double end_radius = 0; //末尾距离
    double end_azimuth = 0; //末尾方位
    bool is_end = false; //是否结束
    bool is_marked = false; //是否被标记
};

/**
 * 单条雷达航迹数据类
 */
class SingleRadarTrackData: public QwtSeriesData<QwtPointPolar> {
  public:
    SingleRadarTrackData();

  public slots:
    //添加点
    void AddTrackPoint(const QwtPointPolar& polar);
    //离指定点的最小距离
    double MinDistance(const QPointF& point);

  private:
    //返回总点数
    virtual size_t size() const;
    //采样，返回从0~(size-1)的每个点
    virtual QwtPointPolar sample(size_t i) const;
    //返回绘图区域
    virtual QRectF boundingRect() const;

    //数据列表
    QList<QwtPointPolar> data_all_;
};

/**
 * 单条航迹末尾点类
 */
class SingleRadarTrackEnd: public QwtPolarMarker {
  public:
    SingleRadarTrackEnd();

  public slots:
    //设置标志颜色
    void SetSymbolColor(const QColor& symbol_color);
    //设置标志尺寸
    void SetSymbolSize(const QSize& symbol_size);
    //设置文字
    void SetText(const QString& text);
    //设置文字颜色
    void SetTextColor(const QColor& text_color);

  private slots:
    //初始化
    void InitAll();
    //初始化标志
    void InitSymbol();
    //初始化文字
    void InitText();

  private:
    QwtSymbol* symbol_ = nullptr;
    QwtText text_;
};

/**
 * 单条雷达航迹类
 */
class SingleRadarTrack {
  public:
    explicit SingleRadarTrack(QwtPolarPlot* plot);
    ~SingleRadarTrack();

  public slots:
    //添加航迹点
    void AddTrackPoint(const RadarTrackInfo& info);
    //到指定点的最小距离
    double MinDistance(const QPointF& point);
    //设置是否标记
    void SetMarked(bool is_marked);
    //是否标记
    bool IsMarked();
    //设置航迹颜色
    void SetTrackColor(const QColor& color);

  private slots:
    //初始化
    void InitAll();
    //初始化标志
    void InitSymbol();
    //初始化曲线
    void InitCurve();
    //初始化末尾点
    void InitTrackEnd();

  private:
    //QwtPolar绘图曲线
    QwtPolarCurve* track_curve_ = nullptr;
    //绘图所用数据点集合
    SingleRadarTrackData* track_data_ = nullptr;
    //标志点
    QwtSymbol* track_symbol_ = nullptr;
    //末尾点
    SingleRadarTrackEnd* track_end_ = nullptr;

    //未标记时颜色
    QColor color_ = Qt::yellow;
    //标记时颜色
    QColor color_marked_ = Qt::blue;
    //未标记时标牌文字
    QString title_ = "";
    //标记时标牌文字
    QString title_marked_ = "";

    //航迹信息
    RadarTrackInfo info_;
};

/**
 * 雷达航迹类
 */
class RadarTracks {
  public:
    explicit RadarTracks(QwtPolarPlot* plot);
    ~RadarTracks();

  public slots:
    //添加航迹
    void AddTrackPoint(quint16 index, const RadarTrackInfo& info);
    //删除指定批号航迹
    void RemoveTrack(quint16 index);
    //标记航迹
    void MarkTrack(const QPointF& point);

  private:
    //画布
    QwtPolarPlot* plot_ = nullptr;
    //航迹
    QMap<quint16, SingleRadarTrack*> radar_tracks;
};

#endif // RADARTRACKS_H
