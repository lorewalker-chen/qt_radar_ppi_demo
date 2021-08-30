#include "radar_track_marker.h"

#include "qwt_symbol.h"

RadarTrackMarker::RadarTrackMarker() {
    //初始化
    InitAll();
}
//设置文字内容
void RadarTrackMarker::SetText(const QString& text) {
    text_.setText(text);
    setLabel(text_);
}
//设置所有颜色
void RadarTrackMarker::SetColor(const QColor& color) {
    SetSymbolColor(color);
    SetTextColor(color);
}
//设置标志颜色
void RadarTrackMarker::SetSymbolColor(const QColor& symbol_color) {
    //设置轮廓颜色
    symbol_->setPen(symbol_color);
    //设置填充颜色
    symbol_->setBrush(QBrush(symbol_color));
}
//设置文字颜色
void RadarTrackMarker::SetTextColor(const QColor& text_color) {
    text_.setColor(text_color);
    setLabel(text_);
}
//初始化
void RadarTrackMarker::InitAll() {
    InitSymbol();//初始化标志
    InitText();//初始化文字
}
//初始化标志
void RadarTrackMarker::InitSymbol() {
    symbol_ = new QwtSymbol;
    //圆形
    symbol_->setStyle(QwtSymbol::Ellipse);
    //尺寸
    symbol_->setSize(QSize(4, 4));
    setSymbol(symbol_);
}
//初始化文字
void RadarTrackMarker::InitText() {
    //字体
    text_.setFont(QFont("Times New Roman", 10));
    setLabel(text_);
    //位置，右上
    setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
}
