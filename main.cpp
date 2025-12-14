#include "mainwindow.hpp"

#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addOption({"lang", "Language", "ru|en"});
    parser.process(a);

    QString lang = parser.value("lang");
    if (lang.isEmpty()) lang = "ru";  // По умолчанию русский

    QTranslator translator;
    QString qmFile = QString(":/translations/minesweeper_%1").arg(lang);

    MainWindow w;
    w.show();

    return a.exec();
}
