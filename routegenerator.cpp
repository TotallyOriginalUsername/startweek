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


Point::Point(int64_t x, int64_t y, float cost)
  : x(x), y(y), cost(cost)
{}

bool Point::operator==(const Point& other) const {
    return x    == other.x
        && y    == other.y
        && cost == other.cost;
}

vector<vector<int>>   routeIndices;
vector<vector<Point>> selectedPoints;
vector<Point>         allPoints;

struct Edge {
    int u, v;
    float weight;
};

// Bereken afstand in meters tussen twee microdegree-punten

double geoDistanceMeters(const Point& a, const Point& b) {
    double dx_deg = (a.x - b.x) * 1e-6;
    double dy_deg = (a.y - b.y) * 1e-6;
    double dx_m  = dx_deg * METERS_PER_DEGREE;
    double dy_m  = dy_deg * METERS_PER_DEGREE;
    return sqrt(dx_m*dx_m + dy_m*dy_m);
}

// Bereken tijd in minuten tussen twee punten

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

void saveRoutesToFiles(const vector<vector<int>>& routes,
                       const vector<vector<Point>>& selPts,
                       const vector<Point>& allPts)
{
    for (size_t i = 0; i < routes.size(); ++i) {
        string filename = "Route_" + to_string(i+1) + ".json";
        ofstream file(filename);
        if (!file) {
            cerr << "Kon bestand niet openen: " << filename << "\n";
            continue;
        }
        file << "[\n";
        for (size_t j = 0; j < routes[i].size(); ++j) {
            const Point& p = selPts[i][routes[i][j]];
            auto it = find(allPts.begin(), allPts.end(), p);
            int mg_id = it != allPts.end() ? static_cast<int>(distance(allPts.begin(), it)) : -1;
            file << "  { \"x\": " << p.x
                 << ", \"y\": " << p.y
                 << ", \"mg_id\": " << mg_id << " }"
                 << (j+1 < routes[i].size() ? "," : "") << "\n";
        }
        file << "]\n";
        cout << "Opgeslagen in '" << filename << "'.\n";
    }
}

void generate_routes() {
    // definieer alle punten met microdegrees en kosten
    allPoints = {
        {51688573, 5287210, 2.0f},  // Avans
        {51690224, 5296625, 1.5f},  // Jan de Groot
        {51688460, 5303150, 2.0f},  // Stadhuis
        {51686200, 5304500, 1.5f},  // Sint Jans Kathedraal
        {51685051, 5289156, 2.0f},  // Paleisbrug
        {51684258, 5302611, 1.5f},  // Zuidwal
        {51691299, 5303950, 2.0f},  // Arena
        {51689428, 5310484, 1.5f},  // Nationaal Carnavalsmuseum
        {51695984, 5299074, 2.0f},  // Tramkade
        {51689124, 5303969, 1.5f},  // De Markt
        {51689619, 5299065, 2.0f},  // Bolwerk
        {51693002, 5301264, 1.5f},  // VUE Cinema
        {51697021, 5299328, 2.0f},  // Bossche Brouwers
        {51689724, 5300408, 1.5f},  // Café Bar le Duc
        {51686471, 5304106, 2.0f},  // Museumkwartier
        {51689471, 5303200, 1.5f},  // Moriaan
        {51689302, 5303396, 2.0f},  // ’t Opkikkertje
        {51695457, 5297448, 1.5f},  // Verkadefabriek
        {51694463, 5302862, 2.0f},  // BHIC
        {51690467, 5294925, 1.5f},  // Station
        {51683776, 5317938, 2.0f},  // Zuiderpark
        {51687561, 5305911, 1.5f},  // Korte Putstraat
        {51698630, 5292803, 1.5f},  // Brabanthallen
        {51688691, 5309001, 1.5f},  // Café de Palm
        {51687344, 5305871, 1.5f},  // Bistro Tante Pietje
        {51696264, 5307460, 1.5f},  // Taxandriaplein Park
        {51696501, 5312884, 1.5f},  // IJzeren Vrouw Park
        {51691911, 5286594, 1.5f},  // Simon Stevinweg
        {51690402, 5291740, 1.5f},  // Hugo de Grootplein
        {51685400, 5289354, 1.5f},  // Kinepolis
        {51690484, 5296206, 1.5f},  // Gouden Draak
        {51686618, 5308459, 1.5f},  // Theater aan de Parade
        {51686697, 5303137, 1.5f}   // Gemeentehuis
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
