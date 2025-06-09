#ifndef ROUTEGENERATOR_H
#define ROUTEGENERATOR_H

#include <vector>

struct Point {
    float x, y;
    float cost = 1.0f;

    Point(float x, float y, float cost = 1.0f) : x(x), y(y), cost(cost) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

void generate_routes();
std::vector<std::vector<Point>> getGeneratedRoutes();
std::vector<std::vector<int>> getGeneratedIndices();
std::vector<Point> getAllPoints();

extern std::vector<std::vector<int>> routeIndices;
extern std::vector<std::vector<Point>> selectedPoints;
extern std::vector<Point> allPoints;
#endif // ROUTEGENERATOR_H
