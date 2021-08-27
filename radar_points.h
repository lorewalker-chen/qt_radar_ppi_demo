#ifndef RADARPOINTS_H
#define RADARPOINTS_H

#include "qwt_series_data.h"

#include "qwt_polar_plot.h"
#include "qwt_polar_curve.h"


/**
 * 雷达点迹数据类
 */
class RadarPointsData: public QwtSeriesData<QwtPointPolar> {
  public:
    RadarPointsData();

  public slots:
    //添加点
    void AddPoint(quint16 cpi, const QwtPointPolar& polar);
    //删除一个cpi周期的点
    void RemovePoints(quint16 cpi);
    //删除全部点
    void RemoveAll();

    //设置点迹容量
    void SetCapacity(int capacity);
    //获取点迹容量
    int Capacity();

  private:
    //将带cpi的数据重组成一个列表
    void RecombineData();
    //返回总点数
    virtual size_t size() const;
    //采样，返回从0~(size-1)的每个点
    virtual QwtPointPolar sample(size_t i) const;
    //返回绘图区域
    virtual QRectF boundingRect() const;

    //带cpi的数据
    QMap<quint16, QList<QwtPointPolar>> data_cpi_;
    //所有数据列表,为了减少采样的时间复杂度
    QList<QwtPointPolar> data_all_;

    //点迹容量
    int capacity_ = -1;
    //点迹当前数量
    int count_ = 0;
};

/**
 * 雷达点迹类
 */
class RadarPoints {
  public:
    explicit RadarPoints(QwtPolarPlot* plot);
    ~RadarPoints();

  public slots:
    //添加点
    void AddPoint(quint16 cpi, const QwtPointPolar& polar);
    //删除一个cpi周期的点
    void RemovePoints(quint16 cpi);
    //删除全部点
    void RemoveAll();
    /**
     * 设置是否根据cpi自动删除，
     * true：interval有效，自动删除该间隔值之前的点。
     * false：interval无效。
     */
    void SetAutoRemoveByCpi(bool enabled, quint16 interval = 20);
    //设置点迹颜色
    void SetColor(const QColor& color);
    //设置点尺寸
    void SetSize(const QSize& size);
    //设置点迹容量
    void SetCapacity(int capacity);
    //获取点迹容量
    int Capacity();

  private slots:
    //初始化
    void InitAll();
    //初始化标志
    void InitSymbol();
    //初始化曲线
    void InitCurve();

  private:
    //QwtPolar绘图曲线
    QwtPolarCurve* points_curve_ = nullptr;
    //绘图所用数据点集合
    RadarPointsData* points_data_ = nullptr;
    //标志点
    QwtSymbol* points_symbol_ = nullptr;

    //是否根据cpi自动删除
    bool is_auto_remove_by_cpi_ = false;
    //每多少cpi删除
    quint16 cpi_remove_interval_ = 20;
    //待删除的cpi
    quint16 cpi_be_removed_ = 0;
};
#endif // RADARPOINTS_H
