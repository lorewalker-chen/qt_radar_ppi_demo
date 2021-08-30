#ifndef RADARTRACKMARKER_H
#define RADARTRACKMARKER_H

#include "qwt_polar_marker.h"

class RadarTrackMarker: public QwtPolarMarker {
  public:
    RadarTrackMarker();

    //内容设置
    void SetText(const QString& text);
    //颜色设置
    void SetColor(const QColor& color);//所有颜色
    void SetSymbolColor(const QColor& symbol_color);//标志颜色
    void SetTextColor(const QColor& text_color);//文字颜色

  private :
    //初始化
    void InitAll();
    void InitSymbol();//初始化标志
    void InitText();//初始化文字

    //标志
    QwtSymbol* symbol_ = nullptr;
    //文字
    QwtText text_;
};

#endif // RADARTRACKMARKER_H
