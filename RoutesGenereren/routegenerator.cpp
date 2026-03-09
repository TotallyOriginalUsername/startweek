#include "routegenerator.h"

#include <qglobal.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdint>

using namespace std;

const int NUM_HOTPOINTS            = 5;
const int NUM_ROUTES               = 10;
const float MIN_ROUTE_TIME_MINUTES = 1.0f * 60.0f;  // 60 min
const float MAX_ROUTE_TIME_MINUTES = 2.0f * 60.0f;  // 120 min

constexpr double METERS_PER_DEGREE    = 111320.0;   // gemiddeld
constexpr double WALKING_SPEED_M_PER_S = 1.4;       // meter per seconde


Point::Point(int loc_id, int64_t x, int64_t y, float cost, int mg_id)
  : loc_id(loc_id), x(x), y(y), cost(cost), mg_id(mg_id)
{}

bool Point::operator==(const Point& other) const {
    return x    == other.x
        && y    == other.y
        && cost == other.cost
        && mg_id == other.mg_id;
}

vector<vector<int>>   routeIndices;
vector<vector<Point>> selectedPoints;
vector<Point>         allPoints;

struct Edge {
    int u, v;
    float weight;
};

// Calculate distance in metres between two microdegree points
double geoDistanceMeters(const Point& a, const Point& b) {
    double dx_deg = (a.x - b.x) * 1e-6;
    double dy_deg = (a.y - b.y) * 1e-6;
    double dx_m  = dx_deg * METERS_PER_DEGREE;
    double dy_m  = dy_deg * METERS_PER_DEGREE;
    return sqrt(dx_m*dx_m + dy_m*dy_m);
}

// Calculate time in minutes between two points
double stepTimeMinutes(const Point& a, const Point& b) {
    double dist_m = geoDistanceMeters(a, b);
    return (dist_m / WALKING_SPEED_M_PER_S) / 60.0;
}

int findSet(int x, vector<int>& parent) {
    return parent[x] == x ? x : parent[x] = findSet(parent[x], parent);
}

float totalRouteCost(const vector<int>& route, const vector<Point>& points) {
    float total = 0.0f;
    for (size_t i = 0; i + 1 < route.size(); ++i) {
        total += static_cast<float>(stepTimeMinutes(points[route[i]], points[route[i+1]]))
               + points[route[i]].cost;
    }
    total += points[route.back()].cost;
    return total;
}

vector<Edge> computeMST(vector<Point>& points) {
    size_t n = points.size();
    vector<Edge> edges, mst;
    vector<int> parent(n);
    for (size_t i = 0; i < n; ++i) parent[i] = i;

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            double d = geoDistanceMeters(points[i], points[j]);
            edges.push_back({ static_cast<int>(i), static_cast<int>(j), static_cast<float>(d) });
        }
    }
    sort(edges.begin(), edges.end(), [](auto& a, auto& b){ return a.weight < b.weight; });
    for (auto& e : edges) {
        int u = findSet(e.u, parent);
        int v = findSet(e.v, parent);
        if (u != v) {
            mst.push_back(e);
            parent[u] = v;
            if (mst.size() == n - 1) break;
        }
    }
    return mst;
}

void optimizeRoute(vector<int>& route, vector<Point>& points) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 1; i + 2 < route.size(); ++i) {
            for (size_t j = i + 1; j + 1 < route.size(); ++j) {
                double d1 = geoDistanceMeters(points[route[i-1]], points[route[i]])
                          + geoDistanceMeters(points[route[j]],   points[route[j+1]]);
                double d2 = geoDistanceMeters(points[route[i-1]], points[route[j]])
                          + geoDistanceMeters(points[route[i]],   points[route[j+1]]);
                if (d2 < d1) {
                    reverse(route.begin()+i, route.begin()+j+1);
                    improved = true;
                }
            }
        }
    }
}

vector<int> christofides(vector<Point>& points) {
    auto mst = computeMST(points);
    vector<int> degree(points.size(), 0);
    for (auto& e : mst) {
        degree[e.u]++; degree[e.v]++;
    }
    vector<int> odd;
    for (size_t i = 0; i < points.size(); ++i)
        if (degree[i] % 2) odd.push_back(static_cast<int>(i));

    vector<Edge> matching;
    while (odd.size() >= 2) {
        int u = odd.back(); odd.pop_back();
        int v = odd.back(); odd.pop_back();
        matching.push_back({u, v, static_cast<float>(geoDistanceMeters(points[u], points[v]))});
    }

    vector<Edge> eulerian = mst;
    eulerian.insert(eulerian.end(), matching.begin(), matching.end());

    vector<int> route;
    vector<bool> used(points.size(), false);
    route.push_back(0);
    used[0] = true;
    for (auto& e : eulerian) {
        if (!used[e.v]) {
            route.push_back(e.v);
            used[e.v] = true;
        }
    }
    route.push_back(route.front());
    optimizeRoute(route, points);
    return route;
}

void generate_routes(const std::vector<Point> &inputPoints, std::vector<std::vector<int>>& routes) {
    std::vector<Point> allPoints = inputPoints;
    routes.clear();

    vector<Point> hotpoints(allPoints.begin(), allPoints.begin() + NUM_HOTPOINTS);
    vector<Point> waypoints(allPoints.begin() + NUM_HOTPOINTS, allPoints.end());

    random_device rd;
    mt19937 g(rd());

    selectedPoints.clear();
    routeIndices.clear();
    vector<float> routeTimes;
    bool valid = false;

    while (!valid) {
        selectedPoints = vector<vector<Point>>(NUM_ROUTES, hotpoints);
        routeIndices.clear();
        routeTimes.clear();

        shuffle(waypoints.begin(), waypoints.end(), g);
        for (size_t i = 0; i < waypoints.size(); ++i) {
            int rid = static_cast<int>(i % NUM_ROUTES);
            selectedPoints[rid].push_back(waypoints[i]);
            for (int r = 0; r < NUM_ROUTES; ++r) {
                if (rand() % 10 < 3)
                    selectedPoints[r].push_back(waypoints[i]);
            }
        }

        for (int i = 0; i < NUM_ROUTES; ++i) {
            routeIndices.push_back(christofides(selectedPoints[i]));
            routeTimes.push_back(totalRouteCost(routeIndices[i], selectedPoints[i]));
        }

        valid = true;
        for (auto t : routeTimes) {
            if (t < MIN_ROUTE_TIME_MINUTES || t > MAX_ROUTE_TIME_MINUTES) {
                valid = false;
                break;
            }
        }
    }

    routes.resize(NUM_ROUTES);
    for (int r = 0; r < NUM_ROUTES; ++r) {
        const auto& route = routeIndices[r];
        const auto& pts   = selectedPoints[r];
        routes[r].reserve(route.size());
        for (int index : route) {
            routes[r].push_back(pts[index].loc_id);
        }
    }

    for (int i = 0; i < NUM_ROUTES; ++i) {
        cout << "Route " << i+1 << " tijd: "
             << fixed << setprecision(1) << routeTimes[i] << " min\n";
    }
}
