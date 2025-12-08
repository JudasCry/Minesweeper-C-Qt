#pragma once

#include <QMainWindow>
#include "gamewindow.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    Ui::MainWindow *ui;
    GameWindow* gameWindow;

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_startGameButton_clicked();

};
