#pragma once

#include <QMainWindow>
#include "settings.hpp"
#include "statistics.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    std::unique_ptr<Ui::MainWindow> ui;
    std::shared_ptr<Settings> currentSettings;
    std::shared_ptr<Statistics> gameStatistics;

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void applySettings();

private slots:

    void on_startGameButton_clicked();
    void on_settingsButton_clicked();
    void on_statisticsButton_clicked();
    void on_quitButton_clicked();

};
