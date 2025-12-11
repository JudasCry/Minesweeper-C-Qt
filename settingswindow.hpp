#pragma once

#include <QDialog>
#include "settings.hpp"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

private:

    struct ThemeInfo {
        QString id;      // "default", "dark"
        QString name;    // "Стандартная", "Темная"
        QString imagePath; // ":/themes/default.png"
    };

private:

    std::unique_ptr<Ui::SettingsWindow> ui;
    std::shared_ptr<Settings> currentSettings;
    std::shared_ptr<Settings> tempSettings;

    QVector<ThemeInfo> themes;
    int currentThemeIndex = 0;

public:

    explicit SettingsWindow(std::shared_ptr<Settings> currentSettings, QWidget* parent = nullptr);
    ~SettingsWindow();

    void closeEvent(QCloseEvent* event);

    void initThemes();
    void updateThemeDisplay();
    void applyThemePreview();

    void setupUI();
    void setupConnections();
    void loadSettingsToUI();

private slots:

    void on_cancelButton_clicked();
    void on_okButton_clicked();
    void on_defaultsButton_clicked();
    void on_backButton_clicked();
    void on_nextButton_clicked();

signals:

    void settingsChanged();
    void windowClosed();

};
