#include "main_window.h"
#include "ui_main_window.h"

#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ppi_ = new PlanPositionIndicator(ui->widget_ppi);
    //设置ppi量程
    ppi_->SetRange(1600);
    //航迹
    ppi_->SetTracksColor(Qt::green);
    ppi_->SetTracksMarkedColor(Qt::white);
    //开启自动刷新航迹
    ppi_->SetAutoClearTracksByTime(false, 10000);

    //开启自动刷新点迹
    ppi_->SetAutoClearPointsByCpi(true);
    //设置点迹颜色
    ppi_->SetPointsColor(Qt::red);

    //表格
    model_ = new RadarTrackTableModel;
    ui->tableView->setModel(model_);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    connect(ppi_, &PlanPositionIndicator::MarkedTrack, model_, &RadarTrackTableModel::MarkItem);
    connect(ppi_, &PlanPositionIndicator::RemovedTrack, model_, &RadarTrackTableModel::RemoveItem);

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
    model_->deleteLater();

    delete ui;
}

void MainWindow::AddPoint() {
    ppi_->AddPoint(point_cpi_, point_r_, point_a_);
    point_cpi_ = point_r_ / 100;
    point_r_ += 10;
    point_a_ += 0.1;
    if (point_a_ > 360.0) {
        point_a_ -= 360.0;
    }
}

void MainWindow::AddTrack() {
    info.index = info.last_point_radius / 100;
    info.last_point_radius += 10;
    info.last_point_azimuth += 1;
    if (info.last_point_azimuth > 360.0) {
        info.last_point_azimuth -= 360.0;
    }
    ppi_->AddTrackPoint(info);

    if (info.index == 10) {
        timer_add_track_->stop();
//        ppi_->RemoveTrack(0);
    }
    //添加到表格
    TrackTableItem item;
    item.index = info.index;
    item.distance = info.last_point_radius;
    item.azimuth = info.last_point_azimuth;
    item.type = "人";
    model_->AddItem(item);
}

