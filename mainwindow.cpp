#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QIcon>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)

    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      gameWindow(nullptr)

{

    ui->setupUi(this);

    setWindowIcon(QIcon("C:/QtProjects/MinesweeperProject/mine.ico"));
    setStyleSheet("background-color: #c0c0c0;");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startGameButton_clicked() {

    qDebug() << "Запуск новой игры (Новичок 9×9, 10 мин)";

    Difficulty beginner("Beginner", 9, 9, 10);
    Settings settings;
    auto statistics = std::make_shared<Statistics>(0, 0, 0, 0, 0);

    // Создаём окно с игрой и передаём параметры игры по умолчанию "Новичок" //
    gameWindow = new GameWindow(beginner, settings, statistics, this);

    // Показываем главное меню при закрытии игры //
    connect(gameWindow, &GameWindow::windowClosed, this, &MainWindow::show);

    // Автоудаление окна при закрытии //
    connect(gameWindow, &GameWindow::windowClosed, gameWindow, &QObject::deleteLater);

    gameWindow->show();
    this->hide();

}

