#ifndef ROUTEGENERATOR_H
#define ROUTEGENERATOR_H

#include <vector>

// Verplaats de struct hierheen zodat andere bestanden hem kunnen gebruiken
struct Point {
    float x, y;
    float cost = 1.0f;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

void generate_routes();
std::vector<std::vector<Point>> getGeneratedRoutes();
std::vector<std::vector<int>> getGeneratedIndices();
std::vector<Point> getAllPoints();

#endif // ROUTEGENERATOR_H
