#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QPointer>
#include <memory>
#include "game.hpp"

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:

    static constexpr int DEFAULT_CELL_SIZE = 30; // Размер клетки
    static constexpr int CELL_SPACING = 1; // Отступ м/д клетками
    static constexpr int WINDOW_PADDING_WIDTH = 100; // Отступ окна по ширине
    static constexpr int WINDOW_PADDING_HEIGHT = 200; // Отступ окна по высоте

    // Контейнер для хранения цветов цифр //
    static const QMap<int , QString> NUMBER_COLORS;

private:

    std::unique_ptr<Ui::GameWindow> ui;
     std::unique_ptr<Game> game;
    QVector<QVector<QPointer<QPushButton>>> buttons; // Двумерный массив для клеток

public:

    explicit GameWindow(const Difficulty& currentDifficulty,
                        const Settings& settings,
                        std::shared_ptr<Statistics> statistics,
                        QWidget *parent = nullptr);
    ~GameWindow();

    void closeEvent(QCloseEvent* event);

    void onLeftClick(int x, int y);
    void onRightClick(int x, int y);

    void updateCell(int x, int y);
    void updateField();

    QPushButton* getButtonAt(int x, int y);

    void createGameField();
    void setupWindowSize(int width, int height, int cellSize);
    int calculateOptimalCellSize(int width, int height) const;

private slots:

    void on_restartButton_clicked();

signals:

    void windowClosed();

};
