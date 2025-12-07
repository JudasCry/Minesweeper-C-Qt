#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QIcon>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)

    : QMainWindow(parent),
      ui(new Ui::MainWindow)

{

    ui->setupUi(this);

    setWindowIcon(QIcon("C:/QtProjects/MinesweeperProject/mine.ico"));
    setStyleSheet("background-color: #c0c0c0;");

}

MainWindow::~MainWindow()
{
    delete ui;
}
