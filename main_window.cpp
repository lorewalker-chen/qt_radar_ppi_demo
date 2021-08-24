#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ppi_ = new PlanPositionIndicator(ui->widget_ppi);
}

MainWindow::~MainWindow() {
    delete ui;
}

