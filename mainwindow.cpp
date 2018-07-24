#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "LaunchProcess.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->LaunchPushButton, SIGNAL(pressed()), this, SLOT(launch()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::launch()
{
    ui->resultTextEdit->append("Launch");
}
