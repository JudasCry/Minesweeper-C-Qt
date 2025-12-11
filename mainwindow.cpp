#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QIcon>
#include <QDebug>
#include "gamewindow.hpp"
#include "settingswindow.hpp"

MainWindow::MainWindow(QWidget *parent)

    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      currentSettings(std::make_shared<Settings>()),
      gameStatistics(std::make_shared<Statistics>(0, 0, 0, 0, 0))

{

    ui->setupUi(this);

    applySettings();

    setWindowIcon(QIcon("C:/QtProjects/MinesweeperProject/mine.ico"));
    // setStyleSheet("background-color: #c0c0c0;");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::applySettings() {

    if (!currentSettings) return;

    QString theme = currentSettings->getTheme();

    if (theme == "dark") {

        qApp->setStyleSheet(
            "QMainWindow, QDialog {"
            "    background-color: #2d2d2d;"
            "}"
            "QLabel {"
            "    color: white;"
            "}"
            "QPushButton {"
            "    background-color: #3d3d3d;"
            "    color: white;"
            "    border: 1px solid #555;"
            "}"
            "QRadioButton {"
            "    color: white;"
            "}"
            "QCheckBox {"
            "    color: white;"
            "}"
        );

    } else if (theme == "default") {
        qApp->setStyleSheet("");
    }

}

void MainWindow::on_startGameButton_clicked() {

    Difficulty currentDifficulty("Beginner", 9, 9, 10);

    // Создаём окно с игрой и передаём параметры игры по умолчанию "Новичок" //
    GameWindow* gameWindow = new GameWindow(currentDifficulty, *currentSettings, gameStatistics, this);

    // Показываем главное меню при закрытии игры //
    connect(gameWindow, &GameWindow::windowClosed, this, &MainWindow::show);

    // Автоудаление окна при закрытии //
    connect(gameWindow, &GameWindow::windowClosed, gameWindow, &QObject::deleteLater);

    gameWindow->show();
    this->hide();

}

void MainWindow::on_settingsButton_clicked() {

    SettingsWindow* settingsWindow = new SettingsWindow(currentSettings, this);

    // Подключаем сигнал о смене настроек //
    connect(settingsWindow, &SettingsWindow::settingsChanged, this, &MainWindow::applySettings);

    // Показываем главное меню при закрытии настроек //
    connect(settingsWindow, &SettingsWindow::windowClosed, this, &MainWindow::show);

    settingsWindow->show();
    this->hide();
}

void MainWindow::on_statisticsButton_clicked() {



}

void MainWindow::on_quitButton_clicked() {
    close();
}
