#include "radar_tracks.h"
#include "qwt_symbol.h"

/**
 * 单条雷达航迹数据类
 */
SingleRadarTrackData::SingleRadarTrackData() {
    data_all_.clear();
}
//添加点
void SingleRadarTrackData::AddTrackPoint(const QwtPointPolar& polar) {
    data_all_.append(polar);
}
//离指定点的最小距离
double SingleRadarTrackData::MinDistance(const QPointF& point) {
    double distance = 200;
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
size_t SingleRadarTrackData::size() const {
    return data_all_.size();
}
//从0~(size-1)的每个点
QwtPointPolar SingleRadarTrackData::sample(size_t i) const {
    return data_all_.at(i);
}
//返回绘图区域
QRectF SingleRadarTrackData::boundingRect() const {
    return qwtBoundingRect(*this);
}

/**
 * 单条航迹末尾点类
 */
SingleRadarTrackEnd::SingleRadarTrackEnd() {
    //初始化
    InitAll();
}
//设置标志颜色
void SingleRadarTrackEnd::SetSymbolColor(const QColor& symbol_color) {
    //设置轮廓颜色
    symbol_->setPen(symbol_color);
    //设置填充颜色
    symbol_->setBrush(QBrush(symbol_color));
}
//设置标志尺寸
void SingleRadarTrackEnd::SetSymbolSize(const QSize& symbol_size) {
    symbol_->setSize(symbol_size);
}
//设置文字
void SingleRadarTrackEnd::SetText(const QString& text) {
    text_.setText(text);
    setLabel(text_);
}
//设置文字颜色
void SingleRadarTrackEnd::SetTextColor(const QColor& text_color) {
    text_.setColor(text_color);
    setLabel(text_);
}
//初始化
void SingleRadarTrackEnd::InitAll() {
    InitSymbol();
    InitText();
}
//初始化标志
void SingleRadarTrackEnd::InitSymbol() {
    symbol_ = new QwtSymbol;
    //圆形
    symbol_->setStyle(QwtSymbol::Ellipse);
    //尺寸
    symbol_->setSize(QSize(4, 4));
    setSymbol(symbol_);
}
//初始化文字
void SingleRadarTrackEnd::InitText() {
    //字体
    text_.setFont(QFont("Times New Roman", 15));
    setLabel(text_);
    //位置，右上
    setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
}

/**
 * 单条雷达航迹类
 */
SingleRadarTrack::SingleRadarTrack(QwtPolarPlot* plot) {
    //初始化
    InitAll();
    //依附到画布
    track_curve_->attach(plot);
    track_end_->attach(plot);
}

SingleRadarTrack::~SingleRadarTrack() {
    track_curve_->detach();
    delete  track_curve_;

    track_end_->detach();
    delete  track_end_;
}
//添加航迹点
void SingleRadarTrack::AddTrackPoint(const RadarTrackInfo& info) {
    //更新航迹信息
    info_ = info;
    //将末尾点信息添加到数据列表
    track_data_->AddTrackPoint(QwtPointPolar(info_.end_azimuth, info_.end_radius));
    //更新末尾点位置
    track_end_->setPosition(QwtPointPolar(info_.end_azimuth, info_.end_radius));
    //如果标牌为空，更新标牌文字
    if ("" == title_) {
        title_ = QString("%1").arg(info_.index);
        track_end_->SetText(title_);
    }
}
//到指定点的最小距离
double SingleRadarTrack::MinDistance(const QPointF& point) {
    return track_data_->MinDistance(point);
}
//设置是否标记
void SingleRadarTrack::SetMarked(bool is_marked) {
    if (is_marked) {
        //如果被标记
        SetTrackColor(color_marked_);
        title_marked_ = QString("批号:%1\nR:%2\nA:%3")
                        .arg(info_.index)
                        .arg(QString::number(info_.end_radius, 'f', 2))
                        .arg(QString::number(info_.end_azimuth, 'f', 2));
        track_end_->SetText(title_marked_);
    } else {
        //如果不标记
        SetTrackColor(color_);
        track_end_->SetText(title_);
    }
    info_.is_marked = is_marked;
}
//是否标记
bool SingleRadarTrack::IsMarked() {
    return info_.is_marked;
}
//设置航迹颜色
void SingleRadarTrack::SetTrackColor(const QColor& color) {
    //标志颜色
    track_symbol_->setPen(color);
    track_symbol_->setBrush(QBrush(color));
    //曲线颜色
    track_curve_->setPen(QPen(color, 2));
    //末尾点颜色
    track_end_->SetSymbolColor(color);
    track_end_->SetTextColor(color);
}
//初始化
void SingleRadarTrack::InitAll() {
    InitSymbol();
    InitCurve();
    InitTrackEnd();
}
//初始化标志
void SingleRadarTrack::InitSymbol() {
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
//初始化曲线
void SingleRadarTrack::InitCurve() {
    track_curve_ = new QwtPolarCurve;
    //线条画笔，颜色宽度
    track_curve_->setPen(QPen(color_, 2));
    //画线
    track_curve_->setStyle(QwtPolarCurve::Lines);
    //设置标志
    track_curve_->setSymbol(track_symbol_);
    //设置数据
    track_data_ = new SingleRadarTrackData;
    track_curve_->setData(track_data_);
}
//初始化末尾点
void SingleRadarTrack::InitTrackEnd() {
    track_end_ = new SingleRadarTrackEnd;
    //初始标志颜色
    track_end_->SetSymbolColor(color_);
    //初始文字颜色
    track_end_->SetTextColor(color_);
}

/**
 * 雷达航迹类
 */
RadarTracks::RadarTracks(QwtPolarPlot* plot) {
    plot_ = plot;
    radar_tracks.clear();
}

RadarTracks::~RadarTracks() {
    for (quint16 key : radar_tracks.keys()) {
        delete radar_tracks[key];
        radar_tracks[key] = nullptr;
    }
    radar_tracks.clear();
}
//添加航迹
void RadarTracks::AddTrackPoint(quint16 index, const RadarTrackInfo& info) {
    //如果不存在该批号，新建一批
    if (!radar_tracks.contains(index)) {
        SingleRadarTrack* track = new SingleRadarTrack(plot_);
        radar_tracks.insert(index, track);
    }
    //添加航迹点
    radar_tracks[index]->AddTrackPoint(info);
}
//删除指定批号航迹
void RadarTracks::RemoveTrack(quint16 index) {
    delete radar_tracks[index];
    radar_tracks[index] = nullptr;
    radar_tracks.remove(index);
}
//标记航迹
void RadarTracks::MarkTrack(const QPointF& point) {
    double min_d = 50;
    int index = -1;
    for (quint16 key : radar_tracks.keys()) {
        double temp_d = radar_tracks.value(key)->MinDistance(point);
        if (temp_d < min_d) {
            min_d = temp_d;
            index = key;
        }
    }
    //如果找到距离小于50的最近的一批航迹，设置标记与之前相反
    if (index >= 0) {
        bool is_marked = radar_tracks.value(index)->IsMarked();
        radar_tracks.value(index)->SetMarked(!is_marked);
    }
}
