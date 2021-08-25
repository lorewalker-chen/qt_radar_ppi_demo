#ifndef PLANPOSITIONINDICATOR_H
#define PLANPOSITIONINDICATOR_H

#include "qwt_polar_plot.h"
#include "qwt_round_scale_draw.h"
#include "qwt_scale_draw.h"
#include "qwt_polar_grid.h"
#include "qwt_polar_panner.h"
#include "qwt_polar_magnifier.h"

class PlanPositionIndicator: public QwtPolarPlot {
    Q_OBJECT

  public:
    explicit PlanPositionIndicator(QWidget* parent);

  private slots:
    void InitAll();//初始化
    void InitStyle();//初始化整体风格
    void InitScale();//初始化刻度
    void InitGrid();//初始化网格
    void InitPanner();
    void InitZoomer();

  private:
    int range_ = 30000;

    QwtPolarGrid grid_;
    QwtPolarPanner* panner_ = nullptr;
    QwtPolarMagnifier* zoomer_ = nullptr;
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
