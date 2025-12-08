#include "gamewindow.hpp"
#include "ui_gamewindow.h"

#include <QMessageBox>
#include <QScreen>

const QMap<int, QString> GameWindow::NUMBER_COLORS = {
    {1, "blue"}, {2, "green"}, {3, "red"},
    {4, "darkblue"}, {5, "darkred"}, {6, "cyan"},
    {7, "black"}, {8, "gray"}
};

GameWindow::GameWindow(const Difficulty& currentDifficulty,
                       const Settings& settings,
                       std::shared_ptr<Statistics> statistics,
                       QWidget* parent)

    : QMainWindow(parent),
      ui(std::make_unique<Ui::GameWindow>()),
      game(std::make_unique<Game>(currentDifficulty, settings, statistics))
{

    ui->setupUi(this);

    createGameField();

    setWindowTitle("Сапёр - " + currentDifficulty.getName());
}

GameWindow::~GameWindow() {}

void GameWindow::createGameField() {

    int width = game->getGameField().getWidth();
    int height = game->getGameField().getHeight();

    const int cellSize = calculateOptimalCellSize(width, height);

    // Создаём массив кнопок //
    buttons.resize(height);
    for (int y = 0; y < height; y++) {
        buttons[y].resize(width);
    }

    // Создаём Layout //
    QGridLayout* grid = new QGridLayout;
    grid->setSpacing(CELL_SPACING);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setAlignment(Qt::AlignCenter);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            QPointer<QPushButton> btn = new QPushButton();

            btn->setFixedSize(cellSize, cellSize);
            btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            buttons[y][x] = btn; // Сохраняем указатель в массив

            // Левый клик по клетке //
            connect(btn, &QPushButton::clicked,
                    [this, x, y]() { onLeftClick(x, y); });

            // Правый клик по клетке //
            btn->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(btn, &QPushButton::customContextMenuRequested,
                    [this, x, y]() { onRightClick(x, y); });

            grid->addWidget(btn, y, x); // Добавляем в Layout

        }
    }

    ui->gameArea->setLayout(grid); // Размещаем поле в окне

    setupWindowSize(width, height, cellSize); // Устанавливаем размер окна
}

// Вычисление оптимального размера клетки //
int GameWindow::calculateOptimalCellSize(int width, int height) const {

    constexpr int MIN_CELL_SIZE = 20;
    constexpr int MEDIUM_CELL_SIZE = 25;
    constexpr int MAX_CELL_SIZE = 30;

    constexpr int LARGE_FIELD = 25;
    constexpr int HUDE_FIELD = 30;

    int cellSize = MAX_CELL_SIZE;

    if (width > HUDE_FIELD || height > HUDE_FIELD) {
        cellSize = MIN_CELL_SIZE;
    }
    else if (width > LARGE_FIELD || height > LARGE_FIELD) {
        cellSize = MEDIUM_CELL_SIZE;
    }

    return cellSize;
}

void GameWindow::setupWindowSize(int width, int height, int cellSize) {

    int fieldWidth = width * cellSize + (width - 1) * CELL_SPACING;
    int fieldHeight = height * cellSize + (height - 1) * CELL_SPACING;

    int minWindowWidth = fieldWidth + WINDOW_PADDING_WIDTH;
    int minWindowHeight = fieldHeight + WINDOW_PADDING_HEIGHT;

    setMinimumSize(minWindowWidth, minWindowHeight);

    adjustSize();
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        move(screenGeometry.center() - rect().center());
    }

}
// Нажатие левой кнопкой мыши //
void GameWindow::onLeftClick(int x, int y) {

    if (!game) return;

    qDebug() << "Левый клик:" << x << y;

    if (game->getGameState() == GameState::Waiting) {
        game->startGame(Point(x, y));
    } else {
        game->cellClick(Point(x, y));
    }

    updateField();

    if (game->getGameState() == GameState::Won) {
        QMessageBox::information(this, "Победа!", "Вы выиграли!");
    } else if (game->getGameState() == GameState::Lost) {
        QMessageBox::information(this, "Поражение", "Мины!");
        game->getGameField().revealAllMines();
        updateField();
    }

}

// Нажатие правой кнопкой мыши //
void GameWindow::onRightClick(int x, int y) {

    if (!game || game->getGameState() != GameState::Running) return;

    qDebug() << "Правый клик:" << x << y;
    game->flagToggle(Point(x, y));
    updateCell(x, y);

}

void GameWindow::on_restartButton_clicked() {

    if (!game) return;

    game->restartGame();

    updateField();

    if (ui->timerLabel) ui->timerLabel->setText("000");
    if (ui->minesLabel) ui->minesLabel->setText(
            QString::number(game->getCurrentDifficulty().getMines()));

}

// Обновление игрового поля //
void GameWindow::updateField()
{
    if (!game) return;

    const auto& field = game->getGameField();
    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            updateCell(x, y);
        }
    }
}

// Обновление клетки //
void GameWindow::updateCell(int x, int y) {

    auto btn = getButtonAt(x, y);
    if (!btn || !game) return;

    const Cell* cell = game->getGameField().getCell(x, y);
    if (!cell) return;

    // Сбрасываем состояние //
    btn->setText("");
    btn->setStyleSheet("");

    if (cell->getIsRevealed()) { // Если клетка открыта

        btn->setEnabled(false);
        btn->setStyleSheet("background: #e0e0e0; border: 1px solid gray;");

        if (cell->getIsMine()) { // Если клетка - мина

            btn->setText("💣");
            btn->setStyleSheet("background: red; color: black; font-size: 16px;");

        } else if (cell->getAdjacentMines() > 0) {

            int mines = cell->getAdjacentMines();
            btn->setText(QString::number(mines));

            QString color = NUMBER_COLORS.value(mines, "black");
            btn->setStyleSheet(QString("color: %1; font-weight: bold;").arg(color));

        }
    } else {

        btn->setEnabled(true);
        btn->setStyleSheet("QPushButton { background: #c0c0c0; border: 2px outset #808080; }"
                           "QPushButton:pressed { border: 2px inset #808080; }");

        if (cell->getIsFlagged()) {
            btn->setText("🚩");
            btn->setStyleSheet("color: red; font-size: 14px;");
        }
    }

}

// Геттер для кнопки (UI) //
QPushButton* GameWindow::getButtonAt(int x, int y)
{
    if (y >= 0 && y < buttons.size() && x >= 0 && x < buttons[y].size()) {
        return buttons[y][x];
    }
    return nullptr;
}

// Событие при закрытии окна //
void GameWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed(); // Уведомляем MainWindow о закрытии окна
    QMainWindow::closeEvent(event); // Закрываем окно
}
