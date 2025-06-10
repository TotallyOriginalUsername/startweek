#include "routegenerator.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

const int NUM_HOTPOINTS            = 5;
const int NUM_ROUTES               = 10;
const float MIN_ROUTE_TIME_MINUTES = 1.0f * 60.0f;  // 60 min
const float MAX_ROUTE_TIME_MINUTES = 2.0f * 60.0f;  // 120 min

constexpr double METERS_PER_DEGREE    = 111320.0;  // gemiddeld
constexpr double WALKING_SPEED_M_PER_S = 1.4;       // meter per seconde


Point::Point(int64_t x, int64_t y, float cost, int mg_id)
  : x(x), y(y), cost(cost), mg_id(mg_id)
{}

bool Point::operator==(const Point& other) const {
    return x    == other.x
        && y    == other.y
        && cost == other.cost
        && mg_id == other.mg_id;
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
        
            file << "  { \"x\": " << p.x
                 << ", \"y\": " << p.y
                 << ", \"mg_id\": " << p.mg_id << " }"
                 << (j+1 < routes[i].size() ? "," : "") << "\n";
        }
        file << "]\n";
        cout << "Opgeslagen in '" << filename << "'.\n";
    }
}

void generate_routes() {
    // definieer alle punten met microdegrees en kosten
    allPoints = {
        {51688573, 5287210, 2.0f, 0},  // Avans
        {51690224, 5296625, 1.5f, 101},  // Jan de Groot
        {51688460, 5303150, 2.0f, 112},  // Stadhuis
        {51686200, 5304500, 1.5f, 1},  // Sint Jans Kathedraal
        {51685051, 5289156, 2.0f, 9},  // Paleisbrug
        {51684258, 5302611, 1.5f, 111},  // Zuidwal
        {51691299, 5303950, 2.0f, 5},  // Arena
        {51689428, 5310484, 1.5f, 100},  // Nationaal Carnavalsmuseum
        {51695984, 5299074, 2.0f, 113},  // Tramkade
        {51689124, 5303969, 1.5f, 108},  // De Markt
        {51689619, 5299065, 2.0f, 114},  // Bolwerk
        {51693002, 5301264, 1.5f, 115},  // VUE Cinema
        {51697021, 5299328, 2.0f, 102},  // Bossche Brouwers
        {51689724, 5300408, 1.5f, 116},  // Café Bar le Duc
        {51686471, 5304106, 2.0f, 110},  // Museumkwartier
        {51689471, 5303200, 1.5f, 109},  // Moriaan
        {51689302, 5303396, 2.0f, 117},  // ’t Opkikkertje
        {51695457, 5297448, 1.5f, 118},  // Verkadefabriek
        {51694463, 5302862, 2.0f, 119},  // BHIC
        {51690467, 5294925, 1.5f, 8},  // Station
        {51683776, 5317938, 2.0f, 11},  // Zuiderpark
        {51687561, 5305911, 1.5f, 120},  // Korte Putstraat
        {51698630, 5292803, 1.5f, 121},  // Brabanthallen
        {51688691, 5309001, 1.5f, 7},  // Café de Palm
        {51687344, 5305871, 1.5f, 122},  // Bistro Tante Pietje
        {51696264, 5307460, 1.5f, 10},  // Taxandriaplein Park
        {51696501, 5312884, 1.5f, 123},  // IJzeren Vrouw Park
        {51691911, 5286594, 1.5f, 2},  // Simon Stevinweg
        {51690402, 5291740, 1.5f, 124},  // Hugo de Grootplein
        {51685400, 5289354, 1.5f, 3},  // Kinepolis
        {51690484, 5296206, 1.5f, 106},  // Gouden Draak
        {51686618, 5308459, 1.5f, 125},  // Theater aan de Parade
        {51686697, 5303137, 1.5f, 6}   // Gemeentehuis
    };

    vector<Point> hotpoints(allPoints.begin(), allPoints.begin() + NUM_HOTPOINTS);
    vector<Point> waypoints(allPoints.begin() + NUM_HOTPOINTS, allPoints.end());

    random_device rd;
    mt19937 g(rd());

    selectedPoints.clear();
    routeIndices.clear();
    vector<float> routeTimes;
    bool valid = false;

    // Blijf genereren totdat alle routes binnen de tijdslimieten vallen
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

        // Controleer tijdslimieten
        valid = true;
        for (auto t : routeTimes) {
            if (t < MIN_ROUTE_TIME_MINUTES || t > MAX_ROUTE_TIME_MINUTES) {
                valid = false;
                break;
            }
        }
    }

    for (int i = 0; i < NUM_ROUTES; ++i) {
        cout << "Route " << i+1 << " tijd: "
             << fixed << setprecision(1) << routeTimes[i] << " min\n";
    }

    saveRoutesToFiles(routeIndices, selectedPoints, allPoints);
}
