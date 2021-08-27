#include "main_window.h"
#include "ui_main_window.h"

#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ppi_ = new PlanPositionIndicator(ui->widget_ppi);
    //开启自动刷新
    ppi_->SetPointsAutoRemoveByCpi(false);
    //设置点迹颜色
    ppi_->SetPointsColor(Qt::red);
    //设置点迹大小
//    ppi_->SetPointsSize(QSize(10, 10));
    //添加点迹定时器
    timer_add_point_ = new QTimer;
    connect(timer_add_point_, &QTimer::timeout, this, &MainWindow::AddPoint);
    timer_add_point_->start(100);

    //添加航迹定时器
    timer_add_track_ = new QTimer;
    connect(timer_add_track_, &QTimer::timeout, this, &MainWindow::AddTrack);
    timer_add_track_->start(100);
}

MainWindow::~MainWindow() {
    timer_add_point_->stop();
    timer_add_point_->deleteLater();

    timer_add_track_->stop();
    timer_add_track_->deleteLater();

    ppi_->deleteLater();
    delete ui;
}

void MainWindow::AddPoint() {
    ppi_->AddPoint(point_cpi_, point_r_, point_a_);
    point_cpi_ = point_r_ / 10;
    point_r_ += 10;
    point_a_ += 0.1;
    if (point_a_ > 360.0) {
        point_a_ -= 360.0;
    }
}

void MainWindow::AddTrack() {
    info.index = info.end_radius / 100;
    info.end_radius += 10;
    info.end_azimuth += 1;
    if (info.end_azimuth > 360.0) {
        info.end_azimuth -= 360.0;
    }
    ppi_->AddTrackPoint(info.index, info);

    if (info.index == 10) {
        ppi_->RemoveTrack(0);
    }
}

