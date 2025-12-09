#include "gamewindow.hpp"
#include "ui_gamewindow.h"

#include <QMessageBox>
#include <QScreen>

const QMap<int, QString> GameWindow::NUMBER_COLORS = {
    {1, "blue"}, {2, "green"}, {3, "red"},
    {4, "darkblue"}, {5, "darkred"}, {6, "cyan"},
    {7, "black"}, {8, "gray"}
};

const QString GameWindow::CLOSED_CELL_STYLE =
    "QPushButton {"
    "background: #c0c0c0;"
    "border-top: 1px solid white;"
    "border-left: 1px solid white;"
    "border-right: 1px solid black;"
    "border-bottom: 1px solid black;"
    "}"
    "QPushButton:pressed {"
    "border: 1px inset gray;"
    "background: #b0b0b0;"
    "}";

const QString GameWindow::OPENED_CELL_STYLE =
    "background: #e0e0e0; border: 1px solid gray;";

const QString GameWindow::MINE_CELL_STYLE =
    "background: red; border: 1px solid darkred;";

GameWindow::GameWindow(const Difficulty& currentDifficulty,
                       const Settings& settings,
                       std::shared_ptr<Statistics> statistics,
                       QWidget* parent)

    : QMainWindow(parent),
      ui(std::make_unique<Ui::GameWindow>()),
      game(nullptr)
{

    ui->setupUi(this);

    game = std::make_unique<Game>(currentDifficulty, settings, statistics); // Создаём игру

    setupUI(currentDifficulty); // Устанавливаем интерфейс

    createGameField(); // Создаём игровое поле

    setupConnections(); // Подключаем сигналы

}

GameWindow::~GameWindow() = default;

// Установка сигналов //
void GameWindow::setupConnections() {

    // Соединяем обновление таймера с таймером в окне //
    connect(&game->getTimer(), &GameTimer::timeUpdated, [this](const QString& time) {
        ui->timerLabel->setText(time);
    });

    // Соединяем кнопку рестарта //
    connect(ui->restartTrigger, &QAction::triggered,  ui->restartButton, &QPushButton::click);

    // Соединяем смену сложности //
    connect(ui->beginnerDifficulty, &QAction::triggered, this, [this]() {
        changeDifficulty("Beginner", 9, 9, 10);
    });

    connect(ui->intermediateDifficulty, &QAction::triggered, this, [this]() {
        changeDifficulty("Intermediate", 16, 16, 30);
    });

    connect(ui->expertDifficulty, &QAction::triggered, this, [this]() {
        changeDifficulty("Expert", 30, 16, 99);
    });

}

// Установка интерфейса //
void GameWindow::setupUI(const Difficulty& currentDifficulty) {

    // Устанавливаем смайлик на кнопку рестарта //
    ui->restartButton->setIcon(QIcon(":/images/smile1.svg"));
    ui->restartButton->setIconSize(QSize(64, 64));
    ui->restartButton->setText("");
    ui->restartButton->setFixedSize(70, 70);

    // Устаналиваем кол-во мин на поле и сбрасываем таймер //
    int mines = currentDifficulty.getMines();
    ui->minesLabel->setText(QString("%1").arg(mines, 3, 10, QChar('0')));
    ui->timerLabel->setText("000");

    setWindowTitle("Сапёр - " + currentDifficulty.getName());

}

// Создание игрового поля //
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

            btn->setStyleSheet(CLOSED_CELL_STYLE);

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
    constexpr int HUGE_FIELD = 30;

    int cellSize = MAX_CELL_SIZE;

    if (width > HUGE_FIELD || height > HUGE_FIELD) {
        cellSize = MIN_CELL_SIZE;
    }
    else if (width > LARGE_FIELD || height > LARGE_FIELD) {
        cellSize = MEDIUM_CELL_SIZE;
    }

    return cellSize;
}

// Изменение размера экрана в зависимости от сложности //
void GameWindow::setupWindowSize(int width, int height, int cellSize) {

    int fieldWidth = width * cellSize + (width - 1) * CELL_SPACING;
    int fieldHeight = height * cellSize + (height - 1) * CELL_SPACING;

    setMinimumSize(0, 0);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    ui->gameArea->setMinimumSize(fieldWidth, fieldHeight);

    adjustSize();

    // Центрируем окно //
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

// Нажатие на кнопку рестарта //
void GameWindow::on_restartButton_clicked() {

    if (!game) return;

    game->restartGame();

    updateField();

    ui->timerLabel->setText("000");
    int mines = game->getCurrentDifficulty().getMines();
    ui->minesLabel->setText(QString("%1").arg(mines, 3, 10, QChar('0')));
}

// Сменить сложность во время игры //
void GameWindow::changeDifficulty(QString name, int width, int height, int mines) {

    if (!game) return;

    auto settings = game->getCurrentSettings();
    auto statistics = game->getStatistics();

    // Очищаем старое поле в UI //
    if (ui->gameArea->layout()) {
        QLayout* oldLayout = ui->gameArea->layout();
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }
    buttons.clear();

    game.reset();

    Difficulty newDiff(name, width, height, mines);
    game = std::make_unique<Game>(newDiff, settings, statistics);

    connect(&game->getTimer(), &GameTimer::timeUpdated, [this](const QString& time) {
        ui->timerLabel->setText(time);
    }); // Переподключаем таймер

    createGameField();

    adjustSize();
    ui->minesLabel->setText(QString("%1").arg(mines, 3, 10, QChar('0')));
    ui->timerLabel->setText("000");
    setWindowTitle("Сапёр - " + name);

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

    // Сбрасываем содержание //
    btn->setIcon(QIcon());
    btn->setText("");

    if (cell->getIsRevealed()) { // Открытая клетка

        btn->setEnabled(false);

        if (cell->getIsMine()) { // Если клетка - мина

            btn->setIcon(QIcon(":/images/mine.svg"));
            btn->setIconSize(QSize(20, 20));
            btn->setStyleSheet(MINE_CELL_STYLE);

        }
        else if (cell->getAdjacentMines() > 0) {

            int m = cell->getAdjacentMines();
            btn->setText(QString::number(m));
            QString color = NUMBER_COLORS.value(m, "black");
            btn->setStyleSheet(OPENED_CELL_STYLE +
                               QString(" color: %1; font-weight: bold;").arg(color));

        }
        else {
            btn->setStyleSheet(OPENED_CELL_STYLE);
        }
    }
    else { // Закрытая клетка

        btn->setEnabled(true);

        if (cell->getIsFlagged()) {
            btn->setText("🚩");
            btn->setStyleSheet(CLOSED_CELL_STYLE + " color: red; font-size: 14px;");
        }
        else {
            btn->setStyleSheet(CLOSED_CELL_STYLE);
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
