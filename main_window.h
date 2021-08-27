#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plan_position_indicator.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private slots:
    void AddPoint();
    void AddTrack();

  private:
    Ui::MainWindow* ui;

    PlanPositionIndicator* ppi_ = nullptr;

    QTimer* timer_add_point_ = nullptr;

    quint16 point_cpi_ = 0;
    double point_r_ = 0;
    double point_a_ = 0;

    QTimer* timer_add_track_ = nullptr;

    RadarTrackInfo info;
};
#endif // MAINWINDOW_H
