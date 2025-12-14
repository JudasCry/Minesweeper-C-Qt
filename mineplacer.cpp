#include "mineplacer.hpp"
#include "gamefield.hpp"
#include <chrono>

MinePlacer::MinePlacer() : randomGenerator(std::chrono::system_clock::now().time_since_epoch().count()) {}

// Расставляем мины на поле //
void MinePlacer::placeMines(GameField& field, int mines, Point safePoint) {

    int placedMines = 0;
    int fieldWidth = field.getWidth();
    int fieldHeight = field.getHeight();

    int safeRadius = (fieldWidth >= LARGE_FIELD_THRESHOLD || fieldHeight >= LARGE_FIELD_THRESHOLD)
                    ? DEFAULT_SAFE_RADIUS_LARGE
                    : DEFAULT_SAFE_RADIUS_SMALL;

    int attempts = 0;
    int maxAttempts = fieldWidth * fieldHeight * MAX_ATTEMPTS_MULTIPLIER;

    while (placedMines < mines && attempts < maxAttempts) {

        Point randomPos = getRandomPosition(fieldWidth, fieldHeight);

        // Ставим мину если не безопасная зона //
        if (!inSafeZone(randomPos, safePoint, safeRadius)
            && canPlaceMine(field, randomPos, safePoint)) {

            Cell* cell = field.getCell(randomPos.getX(), randomPos.getY());

            if (cell) {
                cell->setMine(true);
                placedMines++;
            }

        }

        attempts++;
    }

    field.setTotalMines(placedMines);
}

bool MinePlacer::canPlaceMine(const GameField& field, Point point, Point safePoint) const {

    const Cell* cell = field.getCell(point.getX(), point.getY());

    if (cell && !cell->getIsMine()) {
        return true;
    }

    return false;

}

// Проверка на нахождение точки в безопасной зоне вокруг безопасной точки //
bool MinePlacer::inSafeZone(const Point& point, const Point& safePoint, int radius) const {

    int dx = std::abs(point.getX() - safePoint.getX());
    int dy = std::abs(point.getY() - safePoint.getY());

    return dx <= radius && dy <= radius;
}

Point MinePlacer::getRandomPosition(int maxX, int maxY) {

    // Создаём распределение для равномерного получения случайных чисел //
    std::uniform_int_distribution<int> distX(0, maxX - 1);
    std::uniform_int_distribution<int> distY(0, maxY - 1);

    // Генерируем случайные координаты //
    int x = distX(randomGenerator);
    int y = distY(randomGenerator);

    return Point(x, y);

}
