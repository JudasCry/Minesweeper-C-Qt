#include "statisticswindow.hpp"
#include "ui_statisticswindow.h"

#include <QMessageBox>

StatisticsWindow::StatisticsWindow(std::shared_ptr<Statistics> statistics, QWidget* parent)
    : QDialog(parent),
    ui(std::make_unique<Ui::StatisticsWindow>()),
    gameStatistics(statistics)
{

    ui->setupUi(this);

    setWindowTitle("Статистика");

    setupConnections();

    updateUI();

}

StatisticsWindow::~StatisticsWindow() = default;

void StatisticsWindow::setupConnections() {

    connect(ui->resetButton, &QPushButton::clicked, this, &StatisticsWindow::on_resetButton_clicked);

    // connect(statisticsWindow, &StatisticsWindow::windowClosed, statisticsWindow, &QObject::deleteLater);

}

void StatisticsWindow::updateUI() {

    if (!gameStatistics) return;

    ui->totalGamesLabel->setText(QString::number(gameStatistics->getTotalGames()));
    ui->winsLabel->setText(QString::number(gameStatistics->getWins()));
    ui->lossesLabel->setText(QString::number(gameStatistics->getLosses()));

    double winRate = gameStatistics->getWinRate();
    ui->winRateLabel->setText(QString("%1").arg(winRate, 0, 'f', 1));

    int bestTime = gameStatistics->getBestTime();
    ui->bestTimeLabel->setText(
        bestTime > 0
        ? QString("%1 сек").arg(bestTime)
        : "-"
    );

    double avgTime = gameStatistics->getAverageTime();
    ui->avgTimeLabel->setText(
        avgTime > 0
        ? QString("%1 сек").arg(avgTime, 0, 'f', 1)
        : "-"
    );

}

void StatisticsWindow::on_resetButton_clicked() {

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Сброс статистики",
        "Вы уверены, что хотите сбросить всю статистику?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (gameStatistics) {
            gameStatistics->reset();
            updateUI();
        }
    }

}

void StatisticsWindow::showDialog(std::shared_ptr<Statistics> statistics, QWidget* parent) {

    StatisticsWindow* dialog = new StatisticsWindow(statistics, parent);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();

}

void StatisticsWindow::closeEvent(QCloseEvent *event) {
    emit windowClosed();
    QDialog::closeEvent(event);
}
