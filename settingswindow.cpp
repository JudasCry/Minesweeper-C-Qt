#include "settingswindow.hpp"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(std::shared_ptr<Settings> settings, QWidget* parent)

    : QDialog(parent),
    ui(std::make_unique<Ui::SettingsWindow>()),
    currentSettings(std::move(settings))

{

    ui->setupUi(this);

    // Создаем копию настроек для редактирования //
    tempSettings = currentSettings ? currentSettings->clone() : std::make_shared<Settings>();

    initThemes();

    setupUI();
    loadSettingsToUI();
    updateThemeDisplay();

    setupConnections();

}

SettingsWindow::~SettingsWindow() = default;

void SettingsWindow::initThemes()
{
    themes = {
        {"default", "Стандартная", ":/images/smile_default.svg"},
        {"dark", "Темная", ":/images/smile_dark.svg"},
    };
}

void SettingsWindow::setupUI() {

    ui->themeImageLabel->setText("");
    ui->themeImageLabel->setFixedSize(64, 64);
    ui->themeImageLabel->setAlignment(Qt::AlignCenter);

    setWindowTitle("Настройки");

}

// Подключение сигналов //
void SettingsWindow::setupConnections() {

    // Устанавливаем сигналы от радиокнопок, чекбоксов //
    connect(ui->russianRadioButton, &QRadioButton::toggled, [this](bool checked) {
        if (checked && tempSettings)
            tempSettings->setLanguage("ru");
    });

    connect(ui->englishRadioButton, &QRadioButton::toggled, [this](bool checked) {
        if (checked && tempSettings)
            tempSettings->setLanguage("en");
    });

    connect(ui->volumeCheckBox, &QCheckBox::toggled, [this](bool checked) {
        if (tempSettings)
            tempSettings->setSoundEnabled(checked);
    });

}

void SettingsWindow::loadSettingsToUI() {

    if (!tempSettings) return;

    // Загружаем язык //
    QString language = tempSettings->getLanguage();
    if (language == "ru") {
        ui->russianRadioButton->setChecked(true);
    } else {
        ui->englishRadioButton->setChecked(true);
    }

    // Загружаем тему по индексу //
    QString currentThemeId = tempSettings->getTheme();
    currentThemeIndex = 0; // по умолчанию

    for (int i = 0; i < themes.size(); ++i) {
        if (themes[i].id == currentThemeId) {
            currentThemeIndex = i;
            break;
        }
    }

    updateThemeDisplay();

    // Загружаем звук //
    bool soundEnabled = tempSettings->getSoundEnabled();
    ui->volumeCheckBox->setChecked(soundEnabled);

}

void SettingsWindow::updateThemeDisplay()
{

    if (themes.isEmpty() || currentThemeIndex < 0 || currentThemeIndex >= themes.size()) {
        return;
    }

    const ThemeInfo& theme = themes[currentThemeIndex];

    QPixmap pixmap(theme.imagePath); // Загружаем картинку темы

    // Если картинки нет, создаем цветной квадрат //
    if (pixmap.isNull()) {
        pixmap = QPixmap(64, 64);
        if (theme.id == "default") {
            pixmap.fill(QColor("#FFD700"));
        } else if (theme.id == "dark") {
            pixmap.fill(QColor("#2d2d2d"));
        } else {
            pixmap.fill(Qt::white);
        }
    }

    // Масштабируем и показываем //
    pixmap = pixmap.scaled(ui->themeImageLabel->size(),
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);
    ui->themeImageLabel->setPixmap(pixmap);

}

void SettingsWindow::applyThemePreview() {

    QString theme = tempSettings->getTheme();

    if (theme == "dark") {
        this->setStyleSheet(
            "QDialog {"
            "    background-color: #2d2d2d;"
            "}"
            "QLabel {"
            "    color: white;"
            "}"
            "QPushButton {"
            "    background-color: #3d3d3d;"
            "    color: white;"
            "    border: 1px solid #555;"
            "}"
            "QRadioButton {"
            "    color: white;"
            "}"
            "QCheckBox {"
            "    color: white;"
            "}"
            "QGroupBox {"
            "    color: white;"
            "    border: 1px solid #555;"
            "}"
            );
    } else if (theme == "default") {
        // Светлый предпросмотр
        this->setStyleSheet(
            "QDialog {"
            "    background-color: #f0f0f0;"
            "}"
            "QLabel {"
            "    color: black;"
            "}"
            "QPushButton {"
            "    background-color: #e0e0e0;"
            "    color: black;"
            "    border: 1px solid #808080;"
            "}"
            "QRadioButton {"
            "    color: black;"
            "}"
            "QCheckBox {"
            "    color: black;"
            "}"
            "QGroupBox {"
            "    color: black;"
            "    border: 1px solid #808080;"
            "}"
            );
    }

}

// Нажатие на кнопку "Отмена" //
void SettingsWindow::on_cancelButton_clicked() {
    emit windowClosed();
    reject();
}

// Нажатие на кнопку "OK" //
void SettingsWindow::on_okButton_clicked() {

    currentSettings->setLanguage(tempSettings->getLanguage());
    currentSettings->setSoundEnabled(tempSettings->getSoundEnabled());
    currentSettings->setTheme(tempSettings->getTheme());

    currentSettings->save();

    emit settingsChanged();
    emit windowClosed();
    accept();

}

// Нажатие на кнопку "По умолчанию" //
void SettingsWindow::on_defaultsButton_clicked() {

    tempSettings->resetToDefaults();
    loadSettingsToUI();
    applyThemePreview();

}

// Переход к предыдущей теме //
void SettingsWindow::on_backButton_clicked() {

    if (themes.isEmpty()) return;

    currentThemeIndex--;
    if (currentThemeIndex < 0) {
        currentThemeIndex = themes.size() - 1;
    }

    updateThemeDisplay();

    if (tempSettings) {
        tempSettings->setTheme(themes[currentThemeIndex].id);
    }

    applyThemePreview();

}

// Переход к следующей теме //
void SettingsWindow::on_nextButton_clicked() {

    if (themes.isEmpty()) return;

    currentThemeIndex++;
    if (currentThemeIndex >= themes.size()) {
        currentThemeIndex = 0;
    }

    updateThemeDisplay();

    if (tempSettings) {
        tempSettings->setTheme(themes[currentThemeIndex].id);
    }

    applyThemePreview();

}

// Событие при закрытии окна //
void SettingsWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed();
    QDialog::closeEvent(event);
}
