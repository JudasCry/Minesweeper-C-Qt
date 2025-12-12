#pragma once

class Point
{
private:

    int x;
    int y;

public:

    Point(int x, int y);

    int getX() const;
    int getY() const;

    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;

    Point& operator+=(const Point& other);
    Point& operator-=(const Point& other);

};
