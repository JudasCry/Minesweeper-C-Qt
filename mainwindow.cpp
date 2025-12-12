#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QIcon>
#include <QDebug>

#include "gamewindow.hpp"
#include "settingswindow.hpp"
#include "themestyles.hpp"

MainWindow::MainWindow(QWidget *parent)

    : QMainWindow(parent),
      ui(std::make_unique<Ui::MainWindow>()),
      currentSettings(std::make_shared<Settings>()),
      gameStatistics(std::make_shared<Statistics>(0, 0, 0, 0, 0))

{

    ui->setupUi(this);

    applySettings();

    setWindowIcon(QIcon("C:/QtProjects/MinesweeperProject/mine.ico"));
    // setStyleSheet("background-color: #c0c0c0;");

}

MainWindow::~MainWindow() = default;

void MainWindow::applySettings() {

    if (!currentSettings) return;

    QString theme = currentSettings->getTheme();
    qApp->setStyleSheet(ThemeStyles::getStyleSheet(theme));

}

void MainWindow::on_startGameButton_clicked() {

    Difficulty currentDifficulty = Difficulty::beginner();

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
