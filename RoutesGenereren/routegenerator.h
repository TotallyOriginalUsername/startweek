// routegenerator.h


#ifndef ROUTEGENERATOR_H
#define ROUTEGENERATOR_H

#include <cstdint>
#include <vector>

struct Point {
    int loc_id;
    int64_t x;
    int64_t y;
    int mg_id;

    float   cost;

    Point(int loc_id = 0,int64_t x = 0, int64_t y = 0, float cost = 1.0f, int mg_id = -1);

    bool operator==(const Point& other) const;

};


extern std::vector<std::vector<int>>    routeIndices;
extern std::vector<std::vector<Point>>  selectedPoints;
extern std::vector<Point>               allPoints;


void generate_routes(const std::vector<Point>& inputPoints, std::vector<std::vector<int>>& routes);


#endif // ROUTEGENERATOR_H
