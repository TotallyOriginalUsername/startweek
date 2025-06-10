// routegenerator.h


#ifndef ROUTEGENERATOR_H
#define ROUTEGENERATOR_H

#include <cstdint>
#include <vector>

struct Point {
    int64_t x;
    int64_t y;
    float   cost;
    Point(int64_t x = 0, int64_t y = 0, float cost = 1.0f);
    bool operator==(const Point& other) const;
};
extern std::vector<std::vector<int>>    routeIndices;
extern std::vector<std::vector<Point>>  selectedPoints;
extern std::vector<Point>               allPoints;
void generate_routes();

#endif // ROUTEGENERATOR_H
