#include "routegenerator.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <fstream>
#include <string>


using namespace std;

const int NUM_HOTPOINTS = 5;
const int NUM_ROUTES = 10;
const float MAX_DISTANCE_RATIO = 1.15f;
const float WALKING_SPEED_PX_PER_MIN = 84.0f; // 1.4 m/s → 84 px/min

struct Point {
    float x, y;
    float cost = 1.0f; // tijd op locatie in minuten
};

bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

struct Edge {
    int u, v;
    float weight;
};

int find(int x, vector<int>& parent) {
    if (parent[x] == x) return x;
    return parent[x] = find(parent[x], parent);
}

float distance(const Point& a, const Point& b) {
    return static_cast<float>(sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
}

float totalRouteCost(const vector<int>& route, const vector<Point>& points) {
    float total = 0.0f;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        float walk = distance(points[route[i]], points[route[i + 1]]) / WALKING_SPEED_PX_PER_MIN;
        total += walk + points[route[i]].cost;
    }
    total += points[route.back()].cost;
    return total;
}

vector<Edge> computeMST(vector<Point>& points) {
    size_t n = points.size();
    vector<Edge> edges, mst;
    vector<int> parent(n);
    for (int i = 0; i < n; i++) parent[i] = i;

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            edges.push_back({ i, j, distance(points[i], points[j]) });
        }
    }

    sort(edges.begin(), edges.end(), [](Edge a, Edge b) {
        return a.weight < b.weight;
    });

    for (const auto& edge : edges) {
        int setU = find(edge.u, parent);
        int setV = find(edge.v, parent);
        if (setU != setV) {
            mst.push_back(edge);
            parent[setU] = setV;
            if (mst.size() == n - 1) break;
        }
    }

    return mst;
}

void optimizeRoute(vector<int>& route, vector<Point>& points) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 1; i < route.size() - 2; ++i) {
            for (size_t j = i + 1; j < route.size() - 1; ++j) {
                float d1 = distance(points[route[i - 1]], points[route[i]]) +
                    distance(points[route[j]], points[route[j + 1]]);
                float d2 = distance(points[route[i - 1]], points[route[j]]) +
                    distance(points[route[i]], points[route[j + 1]]);
                if (d2 < d1) {
                    reverse(route.begin() + i, route.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
}

vector<int> christofides(vector<Point>& points) {
    vector<Edge> mst = computeMST(points);
    vector<int> degree(points.size(), 0);

    for (auto& edge : mst) {
        degree[edge.u]++;
        degree[edge.v]++;
    }

    vector<int> oddNodes;
    for (int i = 0; i < points.size(); i++) {
        if (degree[i] % 2 == 1) oddNodes.push_back(i);
    }

    vector<Edge> matching;
    while (!oddNodes.empty()) {
        int u = oddNodes.back(); oddNodes.pop_back();
        int v = oddNodes.back(); oddNodes.pop_back();
        matching.push_back({ u, v, distance(points[u], points[v]) });
    }

    vector<Edge> eulerianGraph = mst;
    eulerianGraph.insert(eulerianGraph.end(), matching.begin(), matching.end());

    vector<int> route;
    vector<bool> visited(points.size(), false);
    route.push_back(0);
    visited[0] = true;

    for (auto& edge : eulerianGraph) {
        if (!visited[edge.v]) {
            route.push_back(edge.v);
            visited[edge.v] = true;
        }
    }

    route.push_back(route.front());
    optimizeRoute(route, points);
    return route;
}

void saveRoutesToFiles(const vector<vector<int>>& routes, const vector<vector<Point>>& selectedPoints, const vector<Point>& allPoints) {
    for (int i = 0; i < routes.size(); ++i) {
        string filename = "Route_" + to_string(i + 1) + ".txt";
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Kon bestand niet openen: " << filename << endl;
            continue;
        }

        file << "Route " << i + 1 << ": ";
        for (int index : routes[i]) {
            auto it = find(allPoints.begin(), allPoints.end(), selectedPoints[i][index]);
            if (it != allPoints.end()) {
                int locationID = static_cast<int>(distance(allPoints.begin(), it));
                file << locationID << " ";
            } else {
                file << "X ";
            }
        }

        file << endl;
        file.close();
        cout << "Opgeslagen in '" << filename << "'." << endl;
    }
}

void generate_routes() {
    vector<Point> allPoints = {
        {100, 100, 2.0}, {300, 200, 1.5}, {500, 400, 3.5}, {200, 500, 2.5}, {600, 100, 4.0},
        {150, 300, 1.0}, {400, 350, 1.0}, {250, 50, 3.0}, {550, 250, 2.0}, {700, 150, 2.0},
        {120, 500, 1.5}, {320, 270, 1.0}, {530, 420, 2.5}, {260, 430, 2.0}, {580, 130, 3.0},
        {110, 220, 1.0}, {350, 180, 1.5}, {480, 380, 2.5}, {230, 510, 1.0}, {630, 170, 3.0},
        {50, 450, 1.5}, {450, 100, 2.0}, {370, 320, 1.0}, {670, 220, 2.5}, {280, 370, 1.0},
        {90, 290, 1.0}, {490, 470, 3.5}, {310, 150, 1.0}, {590, 290, 1.5}, {710, 410, 3.0},
        {400, 500, 2.5}, {540, 350, 2.0}, {620, 200, 1.0}, {260, 260, 1.0}, {150, 400, 1.0},
        {340, 530, 2.0}, {680, 300, 2.5}, {130, 140, 1.5}, {470, 240, 1.0}, {700, 500, 3.0},
        {330, 440, 1.0}, {210, 360, 1.0}, {570, 450, 2.0}, {90, 190, 1.0}, {610, 110, 3.0},
        {250, 480, 1.5}, {370, 210, 1.0}, {560, 310, 2.5}, {720, 180, 1.0}, {430, 280, 1.0}
    };

    vector<Point> hotpoints(allPoints.begin(), allPoints.begin() + NUM_HOTPOINTS);
    vector<Point> waypoints(allPoints.begin() + NUM_HOTPOINTS, allPoints.end());

    random_device rd;
    mt19937 g(rd());

    vector<vector<Point>> selectedPoints;
    vector<vector<int>> routes;
    vector<float> routeTimes;
    bool valid = false;

    while (!valid) {
        selectedPoints = vector<vector<Point>>(NUM_ROUTES, vector<Point>(hotpoints));
        routes.clear();
        routeTimes.clear();

        shuffle(waypoints.begin(), waypoints.end(), g);

        for (size_t i = 0; i < waypoints.size(); ++i) {
            int routeIndex = i % NUM_ROUTES;
            selectedPoints[routeIndex].push_back(waypoints[i]);
            for (int r = 0; r < NUM_ROUTES; ++r) {
                if (rand() % 10 < 3) {
                    selectedPoints[r].push_back(waypoints[i]);
                }
            }
        }

        for (int i = 0; i < NUM_ROUTES; ++i) {
            routes.push_back(christofides(selectedPoints[i]));
            routeTimes.push_back(totalRouteCost(routes.back(), selectedPoints[i]));
        }

        float minTime = *min_element(routeTimes.begin(), routeTimes.end());
        float maxTime = *max_element(routeTimes.begin(), routeTimes.end());
        valid = (maxTime / minTime) <= MAX_DISTANCE_RATIO;
    }

    for (int i = 0; i < NUM_ROUTES; ++i) {
        cout << "Route " << i + 1 << " tijd: " << routeTimes[i] << " min" << endl;
    }

    saveRoutesToFiles(routes, selectedPoints, allPoints);
}
