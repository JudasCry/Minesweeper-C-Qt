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

    static const QMap<int , QString> NUMBER_COLORS; // Контейнер для хранения цветов цифр

    static const QString CLOSED_CELL_STYLE; // Стиль для закрытой клетки
    static const QString OPENED_CELL_STYLE; // Стиль для открытой клетки
    static const QString MINE_CELL_STYLE; // Стиль для клетки с миной

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

    void setupConnections();
    void setupUI(const Difficulty& currentDifficulty);

    void closeEvent(QCloseEvent* event);

    void onLeftClick(int x, int y);
    void onRightClick(int x, int y);

    void updateCell(int x, int y);
    void updateField();

    QPushButton* getButtonAt(int x, int y);

    void createGameField();
    void setupWindowSize(int width, int height, int cellSize);
    int calculateOptimalCellSize(int width, int height) const;

    void changeDifficulty(QString name, int width, int height, int mines);

private slots:

    void on_restartButton_clicked();

signals:

    void windowClosed();

};
