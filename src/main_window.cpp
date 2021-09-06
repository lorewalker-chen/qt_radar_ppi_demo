#include "main_window.h"
#include "ui_main_window.h"

#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);


    ppi_ = new PlanPositionIndicator(ui->widget_ppi);

    //PPI
    //设置雷达范围
    ppi_->SetRange(20000);//20km
    ppi_->SetAngleRange(313, 47);

    //设置北向角
    ppi_->SetNorthAngle(90);
    //设置位置
    ppi_->SetPosition(1, 2, 3);

    //点迹设置
    ppi_->SetPointsColor(Qt::white);//点迹颜色，白色
    ppi_->SetAutoClearPointsCpi(10);//每10个cpi自动消点
    ppi_->SetAutoClearPointsByCpi(true);//开启自动消点

    //航迹设置
    ppi_->SetTracksColor(Qt::green);//航迹颜色，绿色
    ppi_->SetTracksMarkedColor(Qt::red);//航迹标记颜色，红色
    ppi_->SetAutoClearTracksTime(5000);//每5s自动消航
    ppi_->SetAutoClearTracksByTime(true);//开启自动消航

    //表格模型
    model_ = new RadarTrackTableModel;
    //关联标记、删除航迹信号槽
    connect(ppi_, &PlanPositionIndicator::MarkedTrack, model_, &RadarTrackTableModel::MarkItem);
    connect(ppi_, &PlanPositionIndicator::RemovedTrack, model_, &RadarTrackTableModel::RemoveItem);

    connect(ppi_, &PlanPositionIndicator::FocusOnTrackPolar, this, &MainWindow::FocusOnPolar);

    //表格模型依附到空间
    ui->tableView->setModel(model_);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
    info.type = "人";
    if (point_a_ > 360.0) {
        point_a_ -= 360.0;
    }

    ppi_->SetDateTime(QDateTime::currentDateTime());
}

void MainWindow::AddTrack() {
    info.index = info.radius / 200;
    info.radius += 10;
    info.azimuth += 1;
    info.type = "人";
    info.time = QTime::currentTime();

    if (info.azimuth > 360.0) {
        info.azimuth -= 360.0;
    }
    ppi_->AddTrackPoint(info);

    if (info.index == 10) {
//        timer_add_track_->stop();
//        ppi_->RemoveTrack(0);
    }
    //添加到表格
    model_->AddItem(info);
}

void MainWindow::FocusOnPolar(double radius, double azimuth) {
    qDebug() << radius << "," << azimuth;
}

