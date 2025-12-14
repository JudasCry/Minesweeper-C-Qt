#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QIcon>
#include <QDebug>

#include "aboutprogram.hpp"
#include "gamewindow.hpp"
#include "howtoplay.hpp"
#include "settingswindow.hpp"
#include "statisticswindow.hpp"
#include "themestyles.hpp"

MainWindow::MainWindow(QWidget *parent)

    : QMainWindow(parent),
      ui(std::make_unique<Ui::MainWindow>()),
      currentDifficulty(Difficulty::beginner()),
      currentSettings(std::make_shared<Settings>()),
      gameStatistics(std::make_shared<Statistics>())

{

    ui->setupUi(this);

    applySettings();
    setupUI();
    setupConnections();

}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {

    setWindowIcon(QIcon("C:/QtProjects/MinesweeperProject/mine.ico"));
    // setStyleSheet("background-color: #c0c0c0;");

}

void MainWindow::setupConnections() {

    connect(ui->restartTrigger, &QAction::triggered, ui->startGameButton, &QPushButton::click);

    connect(ui->settingsTrigger, &QAction::triggered, ui->settingsButton, &QPushButton::click);

    connect(ui->statisticsTrigger, &QAction::triggered, ui->statisticsButton, &QPushButton::click);

    connect(ui->beginnerDifficulty, &QAction::triggered, this, [this]() {
        currentDifficulty = Difficulty::beginner();
        on_startGameButton_clicked();
    });

    connect(ui->intermediateDifficulty, &QAction::triggered, this, [this]() {
        currentDifficulty = Difficulty::intermediate();
        on_startGameButton_clicked();
    });

    connect(ui->expertDifficulty, &QAction::triggered, this, [this]() {
        currentDifficulty = Difficulty::expert();
        on_startGameButton_clicked();
    });

    connect(ui->howToPlayTrigger, &QAction::triggered, this, [this]() {
        HowToPlay::showDialog(this);
    });

    connect(ui->aboutProgramTrigger, &QAction::triggered, this, [this]() {
        AboutProgram::showDialog(this);
    });

}

void MainWindow::applySettings() {

    if (!currentSettings) return;

    QString theme = currentSettings->getTheme();
    qApp->setStyleSheet(ThemeStyles::getStyleSheet(theme));

}

void MainWindow::on_startGameButton_clicked() {

    // Создаём окно с игрой и передаём параметры игры по умолчанию "Новичок" //
    GameWindow* gameWindow = new GameWindow(currentDifficulty, currentSettings, gameStatistics, this);

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
    StatisticsWindow::showDialog(gameStatistics, this);
}

void MainWindow::on_quitButton_clicked() {
    close();
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this); // Переводим UI
        setWindowTitle(tr("Сапёр"));
    }
    QMainWindow::changeEvent(event);
}
