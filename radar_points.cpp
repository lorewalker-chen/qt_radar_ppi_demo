#include "radar_points.h"
#include "qwt_symbol.h"

/**
 * 雷达点迹数据类
 */
RadarPointsData::RadarPointsData() {
    data_cpi_.clear();
    data_all_.clear();
}
//添加点
void RadarPointsData::AddPoint(quint16 cpi, const QwtPointPolar& polar) {
    //如果超过容量,删除第一个点
    if (capacity_ > 0 && count_ > capacity_) {
        data_cpi_.first().removeFirst();
        //计数-1
        count_ -= 1;
    }
    //判断该cpi是否存在
    if (data_cpi_.contains(cpi)) {
        //如果该cpi存在，直接添加
        data_cpi_[cpi].append(polar);
    } else {
        //如果该cpi不存在，添加该cpi的点列表
        QList<QwtPointPolar> list;
        list.append(polar);
        data_cpi_.insert(cpi, list);
    }
    //添加成功，计数+1
    count_ += 1;
    //重组数据
    RecombineData();
}
//删除一个cpi周期的点
void RadarPointsData::RemovePoints(quint16 cpi) {
    //计数减
    count_ -= data_cpi_.value(cpi).size();
    //删除该cpi周期的点
    data_cpi_.remove(cpi);
    //重组数据
    RecombineData();
}
//删除全部点
void RadarPointsData::RemoveAll() {
    //计数清零
    count_ = 0;
    //清空图
    data_cpi_.clear();
    //重组数据
    RecombineData();
}
//设置点迹容量
void RadarPointsData::SetCapacity(int capacity) {
    capacity_ = capacity;
}
//获取点迹容量
int RadarPointsData::Capacity() {
    return capacity_;
}
//为了减少采样的时间复杂度，将带cpi的数据重组成一个列表
void RadarPointsData::RecombineData() {
    data_all_.clear();
    for (quint16 key : data_cpi_.keys()) {
        data_all_.append(data_cpi_.value(key));
    }
}
//返回总点数
size_t RadarPointsData::size() const {
    return count_;
}
//从0~(size-1)的每个点
QwtPointPolar RadarPointsData::sample(size_t i) const {
    return data_all_.at(i);
}
//返回绘图区域
QRectF RadarPointsData::boundingRect() const {
    return qwtBoundingRect(*this);
}

/**
 * 雷达点迹类
 */
RadarPoints::RadarPoints(QwtPolarPlot* plot) {
    //初始化
    InitAll();
    //将曲线依附到图层上
    points_curve_->attach(plot);
}

RadarPoints::~RadarPoints() {
    delete  points_curve_;
}
//添加点
void RadarPoints::AddPoint(quint16 cpi, const QwtPointPolar& polar) {
    //根据cpi自动删除
    if (is_auto_remove_by_cpi_ && (cpi - cpi_be_removed_ > cpi_remove_interval_)) {
        points_data_->RemovePoints(cpi_be_removed_);
        cpi_be_removed_ += 1;
    }
    //添加到数据列表
    points_data_->AddPoint(cpi, polar);
}
//删除一个cpi周期的点
void RadarPoints::RemovePoints(quint16 cpi) {
    points_data_->RemovePoints(cpi);
}
//删除全部点
void RadarPoints::RemoveAll() {
    points_data_->RemoveAll();
}
//设置是否根据cpi自动删除
void RadarPoints::SetAutoRemoveByCpi(bool enabled, quint16 interval) {
    if (enabled) {
        cpi_remove_interval_ = interval;
        is_auto_remove_by_cpi_ = true;
    } else {
        is_auto_remove_by_cpi_ = false;
    }
}
//设置点迹颜色
void RadarPoints::SetColor(const QColor& color) {
    //标志轮廓颜色
    points_symbol_->setPen(color);
    //标志填充颜色
    points_symbol_->setBrush(QBrush(color));
}
//设置点尺寸
void RadarPoints::SetSize(const QSize& size) {
    points_symbol_->setSize(size);
}
//设置点迹容量
void RadarPoints::SetCapacity(int capacity) {
    points_data_->SetCapacity(capacity);
}
//获取点迹容量
int RadarPoints::Capacity() {
    return points_data_->Capacity();
}
//初始化
void RadarPoints::InitAll() {
    InitSymbol();
    InitCurve();
}
//初始化标志
void RadarPoints::InitSymbol() {
    points_symbol_ = new QwtSymbol;
    //圆形
    points_symbol_->setStyle(QwtSymbol::Ellipse);
    //初始颜色，灰色
    QColor color = Qt::gray;
    //设置初始轮廓颜色
    points_symbol_->setPen(color);
    //设置初始填充颜色
    points_symbol_->setBrush(QBrush(color));
    //尺寸
    points_symbol_->setSize(QSize(4, 4));
}
//初始化曲线
void RadarPoints::InitCurve() {
    points_curve_ = new QwtPolarCurve;
    //只画点
    points_curve_->setStyle(QwtPolarCurve::NoCurve);
    //设置标志
    points_curve_->setSymbol(points_symbol_);
    //设置数据
    points_data_ = new RadarPointsData;
    points_curve_->setData(points_data_);
}
