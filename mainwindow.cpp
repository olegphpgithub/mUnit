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
    LaunchProcess *lp = new LaunchProcess();
    QObject::connect(lp, SIGNAL(log(QString)), this, SLOT(log(QString)));
    lp->start();

    ui->resultTextEdit->append("Launch 1");

    //lp->deleteLater();
}

void MainWindow::log(QString logString)
{
    ui->resultTextEdit->append(logString);
}
