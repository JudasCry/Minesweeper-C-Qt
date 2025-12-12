#pragma once

#include <QString>

class ThemeStyles
{
private:

    static QString darkStyleSheet();
    static QString lightStyleSheet();

    static QString darkPreviewStyleSheet();
    static QString lightPreviewStyleSheet();

public:

    ThemeStyles() = delete;

    static QString getStyleSheet(const QString& themeID);
    static QString getPreviewStyleSheet(const QString& themeID);

    static QString getGameClosedCellStyle(const QString& themeID);
    static QString getGameOpenedCellStyle(const QString& themeID);
    static QString getGameMineStyle();

};
