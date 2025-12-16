#include "gamewindow.hpp"
#include "ui_gamewindow.h"

#include <QMessageBox>
#include <QScreen>

#include "aboutprogram.hpp"
#include "howtoplay.hpp"
#include "settingswindow.hpp"
#include "statisticswindow.hpp"
#include "themestyles.hpp"

const QMap<int, QString> GameWindow::NUMBER_COLORS = {
    {1, "blue"}, {2, "green"}, {3, "red"},
    {4, "darkblue"}, {5, "darkred"}, {6, "cyan"},
    {7, "black"}, {8, "gray"}
};

GameWindow::GameWindow(const Difficulty& currentDifficulty,
                       std::shared_ptr<Settings> settings,
                       std::shared_ptr<Statistics> statistics,
                       QWidget* parent)

    : QMainWindow(parent),
      ui(std::make_unique<Ui::GameWindow>()),
      game(nullptr),
      currentSettings(settings)
{

    ui->setupUi(this);

    game = std::make_unique<Game>(currentDifficulty, *currentSettings, statistics); // Создаём игру

    setupUI(currentDifficulty); // Устанавливаем интерфейс

    createGameField(); // Создаём игровое поле

    setupConnections(); // Подключаем сигналы

    updateMenuLanguage();

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
        changeDifficulty(Difficulty::beginner());
    });

    connect(ui->intermediateDifficulty, &QAction::triggered, this, [this]() {
        changeDifficulty(Difficulty::intermediate());
    });

    connect(ui->expertDifficulty, &QAction::triggered, this, [this]() {
        changeDifficulty(Difficulty::expert());
    });

    // Соединяем открытие окна "Настройки" //
    connect(ui->settingsTrigger, &QAction::triggered, this, [this]() {
        showSettingsWindow();
    });

    connect(ui->staticticsTrigger, &QAction::triggered, this, [this]() {
        showStatisticsWindow();
    });

    // Соединяем открытие окна "Как играть" //
    connect(ui->howToPlayTrigger, &QAction::triggered, this, [this]() {
        HowToPlay::showDialog(this);
    });

    // Соединяем открытие окна "О программе" //
    connect(ui->aboutProgramTrigger, &QAction::triggered, this, [this]() {
        AboutProgram::showDialog(this);
    });

}

// Установка интерфейса //
void GameWindow::setupUI(const Difficulty& currentDifficulty) {

    // Устанавливаем смайлик на кнопку рестарта //
    QString theme = currentSettings->getTheme();

    if (theme == "dark") {
        ui->restartButton->setIcon(QIcon(":/images/smile_dark.svg"));
    } else {
        ui->restartButton->setIcon(QIcon(":/images/smile_default.svg"));
    }

    ui->restartButton->setIconSize(QSize(64, 64));
    ui->restartButton->setText("");
    ui->restartButton->setFixedSize(70, 70);

    // Устаналиваем кол-во мин на поле и сбрасываем таймер //
    int mines = currentDifficulty.getMines();
    ui->minesLabel->setText(QString("%1").arg(mines, 3, 10, QChar('0')));
    ui->timerLabel->setText("000");

    setWindowTitle("Сапёр - " + currentDifficulty.getName());

}

// Изменение размера экрана в зависимости от сложности //
void GameWindow::setupWindowSize(int width, int height, int cellSize) {

    int fieldWidth = width * cellSize + (width - 1) * CELL_SPACING;
    int fieldHeight = height * cellSize + (height - 1) * CELL_SPACING;

    ui->gameArea->setFixedSize(fieldWidth, fieldHeight);

    int menuHeight = ui->menubar->height();
    int headerHeight = ui->headerWidget->minimumHeight();
    int statusHeight = ui->statusbar->height();

    int minWindowWidth = ui->centralwidget->minimumSize().width();

    QMargins margins = ui->centralwidget->layout()->contentsMargins();

    int windowWidth = qMax(fieldWidth + margins.left() + margins.right(), minWindowWidth);

    int windowHeight = menuHeight
                       + headerHeight
                       + statusHeight
                       + fieldHeight
                       + margins.top()
                       + margins.bottom();

    setFixedSize(windowWidth, windowHeight);

    // Центрируем окно //
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        move(screenGeometry.center() - rect().center());
    }

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

            QString theme = currentSettings->getTheme();
            btn->setStyleSheet(ThemeStyles::getGameClosedCellStyle(theme));

            buttons[y][x] = btn; // Сохраняем указатель в массив

            // Левый клик по клетке //
            connect(btn, &QPushButton::clicked, [this, x, y]() {
                onLeftClick(x, y);
            });

            // Правый клик по клетке //
            btn->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(btn, &QPushButton::customContextMenuRequested, [this, x, y]() {
                onRightClick(x, y);
            });

            grid->addWidget(btn, y, x); // Добавляем в Layout

        }
    }

    ui->gameArea->setLayout(grid); // Размещаем поле в окне

    if (width * cellSize < 280) {  // Если поле маленькое
        QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
        if (mainLayout) {
            mainLayout->setAlignment(ui->gameArea, Qt::AlignHCenter);
        }
    }

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

    if (!game) return;

    qDebug() << "Правый клик:" << x << y;
    game->flagToggle(Point(x, y));
    updateCell(x, y);
    updateMinesCounter();

}

// Сменить сложность во время игры //
void GameWindow::changeDifficulty(const Difficulty& newDifficulty) {

    if (!game) return;

    auto settings = game->getCurrentSettings();
    auto statistics = game->getStatistics();

    // Очищаем старое поле в UI //
    if (ui->gameArea->layout()) {
        QLayout* oldLayout = ui->gameArea->layout();
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
    }
    buttons.clear();

    // Создаём игру с новой сложностью //
    game = std::make_unique<Game>(newDifficulty, *currentSettings, statistics);

    connect(&game->getTimer(), &GameTimer::timeUpdated, [this](const QString& time) {
        ui->timerLabel->setText(time); // Переподключаем таймер
    });

    createGameField();

    updateMinesCounter();
    ui->timerLabel->setText("000");
    setWindowTitle("Сапёр - " + newDifficulty.getName());

}

// Показать окно настроек //
void GameWindow::showSettingsWindow() {

    SettingsWindow* settingsWindow = new SettingsWindow(currentSettings, this);

    connect(settingsWindow, &SettingsWindow::settingsChanged, this, [this]() {

        QString theme = currentSettings->getTheme();
        QString lang = currentSettings->getLanguage();

        game->setCurrentSettings(*currentSettings);

        theme == "dark" ? ui->restartButton->setIcon(QIcon(":/images/smile_dark.svg"))
                        : ui->restartButton->setIcon(QIcon(":/images/smile_default.svg"));

        updateField();
        qApp->setStyleSheet(ThemeStyles::getStyleSheet(theme));

        emit languageChanged(lang);
        updateMenuLanguage();

        QString diffName = game->getCurrentDifficulty().getName();

        if (lang == "ru") {
            if (diffName == "Beginner") diffName = "Новичок";
            else if (diffName == "Intermediate") diffName = "Любитель";
            else if (diffName == "Expert") diffName = "Эксперт";
            setWindowTitle("Сапёр - " + diffName);
        } else {
            setWindowTitle("Minesweeper - " + diffName);
        }

    });

    connect(settingsWindow, &SettingsWindow::windowClosed, settingsWindow, &QObject::deleteLater);

    settingsWindow->setModal(true);
    settingsWindow->show();

}

// Обновление языка в окне игры
void GameWindow::updateMenuLanguage()
{
    QString lang = currentSettings->getLanguage();

    if (lang == "ru") {
        ui->gameMenu->setTitle("Игра");
        ui->difficultyMenu->setTitle("Сложность");
        ui->helpMenu->setTitle("Помощь");
        ui->beginnerDifficulty->setText("Новичок");
        ui->intermediateDifficulty->setText("Любитель");
        ui->expertDifficulty->setText("Эксперт");
        ui->restartTrigger->setText("Новая игра");
        ui->settingsTrigger->setText("Настройки");
        ui->staticticsTrigger->setText("Статистика");
        ui->howToPlayTrigger->setText("Как играть");
        ui->aboutProgramTrigger->setText("О программе");
    } else if (lang == "en") {
        ui->gameMenu->setTitle("Game");
        ui->difficultyMenu->setTitle("Difficulty");
        ui->helpMenu->setTitle("Help");
        ui->beginnerDifficulty->setText("Beginner");
        ui->intermediateDifficulty->setText("Intermediate");
        ui->expertDifficulty->setText("Expert");
        ui->restartTrigger->setText("New Game");
        ui->settingsTrigger->setText("Settings");
        ui->staticticsTrigger->setText("Statistics");
        ui->howToPlayTrigger->setText("How to Play");
        ui->aboutProgramTrigger->setText("About");
    }
}

// Показать окно статистики //
void GameWindow::showStatisticsWindow() {

    StatisticsWindow* statisticsWindow = new StatisticsWindow(game->getStatistics(), this);

    connect(statisticsWindow, &StatisticsWindow::windowClosed, statisticsWindow, &QObject::deleteLater);

    statisticsWindow->setModal(true);
    statisticsWindow->show();

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

    QString theme = currentSettings->getTheme();

    // Сбрасываем содержание //
    btn->setIcon(QIcon());
    btn->setText("");

    if (cell->getIsRevealed()) { // Открытая клетка

        btn->setEnabled(false);

        if (cell->getIsMine()) { // Если клетка - мина

            btn->setIcon(QIcon(":/images/mine.svg"));
            btn->setIconSize(QSize(20, 20));
            btn->setStyleSheet(ThemeStyles::getGameMineStyle());

        }
        else if (cell->getAdjacentMines() > 0) {

            int countMines = cell->getAdjacentMines();
            btn->setText(QString::number(countMines));

            QString color = NUMBER_COLORS.value(countMines, "black");
            btn->setStyleSheet(ThemeStyles::getGameOpenedCellStyle(theme) +
                               QString(" color: %1; font-weight: bold;").arg(color));

        }
        else {
            btn->setStyleSheet(ThemeStyles::getGameOpenedCellStyle(theme));
        }
    }
    else { // Закрытая клетка

        btn->setEnabled(true);

        if (cell->getIsFlagged()) {
            btn->setText("🚩");
            btn->setStyleSheet(ThemeStyles::getGameClosedCellStyle(theme) + " color: red; font-size: 14px;");
        }
        else {
            btn->setStyleSheet(ThemeStyles::getGameClosedCellStyle(theme));
        }
    }
}

// Обновление счётчика количества мин //
void GameWindow::updateMinesCounter() {

    if (!game) return;

    int mines = game->getCurrentDifficulty().getMines();
    int flags = game->getGameField().getFlagsPlaced();
    int remaining = mines - flags;

    ui->minesLabel->setText(QString("%1").arg(remaining, 3, 10, QChar('0')));

}

// Геттер для кнопки (UI) //
QPushButton* GameWindow::getButtonAt(int x, int y)
{
    if (y >= 0 && y < buttons.size() && x >= 0 && x < buttons[y].size()) {
        return buttons[y][x];
    }
    return nullptr;
}

// Нажатие на кнопку рестарта //
void GameWindow::on_restartButton_clicked() {

    if (!game) return;

    game->restartGame();
    updateField();
    updateMinesCounter();

    ui->timerLabel->setText("000");
}

void GameWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateMenuLanguage();

        QString diffName = tr(game->getCurrentDifficulty().getName().toUtf8());
        setWindowTitle(tr("Сапёр - %1").arg(diffName));
    }
    QMainWindow::changeEvent(event);
}

// Событие при закрытии окна //
void GameWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed(); // Уведомляем MainWindow о закрытии окна
    QMainWindow::closeEvent(event); // Закрываем окно
}
