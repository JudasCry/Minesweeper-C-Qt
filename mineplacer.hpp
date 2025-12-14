#pragma once

#include <random>
#include "point.hpp"

class GameField;

class MinePlacer
{

private:

    static constexpr int DEFAULT_SAFE_RADIUS_SMALL = 1; // 3x3
    static constexpr int DEFAULT_SAFE_RADIUS_LARGE = 2; // 5x5
    static constexpr int LARGE_FIELD_THRESHOLD = 16;
    static constexpr int MAX_ATTEMPTS_MULTIPLIER = 2;

private:

    std::mt19937 randomGenerator;

public:

    MinePlacer();

    void placeMines(GameField& field, int mines, Point safePoint);
    bool canPlaceMine(const GameField& field, Point point, Point safePoint) const;
    bool inSafeZone(const Point& point, const Point& safePoint, int radius) const;
    Point getRandomPosition(int maxX, int maxY);

};
