#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <fstream>

using namespace std;
using namespace sf;

const int WIDTH = 800;
const int HEIGHT = 600;
const int NUM_HOTPOINTS = 5;
const int NUM_ROUTES = 10;
const float MAX_DISTANCE_RATIO = 1.10f;

struct Point {
    float x, y;
    float cost = 1.0f; // standaard tijd op locatie in minuten
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
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

float totalRouteCost(const vector<int>& route, const vector<Point>& points) {
    float total = 0.0f;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        float dist = distance(points[route[i]], points[route[i + 1]]);
        float walkTime = dist / 84.0f;
        total += walkTime + points[route[i]].cost;
    }
    total += points[route.back()].cost;
    return total;
}

vector<Edge> computeMST(vector<Point>& points) {
    int n = points.size();
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

    for (int i = 0; i < NUM_HOTPOINTS; i++) {
        if (find(route.begin(), route.end(), i) == route.end()) {
            int bestInsertPos = 1;
            float bestIncrease = FLT_MAX;
            for (size_t j = 1; j < route.size(); j++) {
                float increase = distance(points[route[j - 1]], points[i]) +
                    distance(points[i], points[route[j]]) -
                    distance(points[route[j - 1]], points[route[j]]);
                if (increase < bestIncrease) {
                    bestIncrease = increase;
                    bestInsertPos = j;
                }
            }
            route.insert(route.begin() + bestInsertPos, i);
        }
    }

    if (route.front() != 0) route.insert(route.begin(), 0);
    if (route.back() != 0) route.push_back(0);

    optimizeRoute(route, points);

    return route;
}

void saveRoutesToFile(const vector<vector<int>>& routes, const vector<vector<Point>>& selectedPoints, const vector<Point>& allPoints, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Kon bestand niet openen: " << filename << endl;
        return;
    }

    for (int i = 0; i < routes.size(); ++i) {
        file << "Route " << i + 1 << ": ";
        for (int index : routes[i]) {
            auto it = find(allPoints.begin(), allPoints.end(), selectedPoints[i][index]);
            if (it != allPoints.end()) {
                int locationID = distance(allPoints.begin(), it);
                file << locationID << " ";
            }
            else {
                file << "X ";
            }
        }
        file << endl;
    }

    file.close();
    cout << "Routes opgeslagen in '" << filename << "'." << endl;
}

void drawGraph(RenderWindow& window, vector<Point>& allPoints, vector<vector<int>>& routes, vector<vector<Point>>& selectedPoints) {
    window.clear(Color::Black);

    for (auto& p : allPoints) {
        CircleShape shape(5);
        shape.setPosition(p.x - 3, p.y - 3);
        shape.setFillColor(Color::White);
        window.draw(shape);
    }

    vector<Color> routeColors = {
        Color::Red, Color::Green, Color::Blue, Color::Cyan,
        Color::Magenta, Color::Yellow, Color(255, 165, 0),
        Color(128, 0, 128), Color(0, 255, 127), Color(255, 105, 180)
    };

    for (int i = 0; i < NUM_ROUTES; ++i) {
        auto& route = routes[i];
        auto& points = selectedPoints[i];
        Color color = routeColors[i % routeColors.size()];

        for (size_t j = 0; j < points.size(); ++j) {
            CircleShape shape(6);
            shape.setPosition(points[j].x - 3, points[j].y - 3);
            shape.setFillColor(j < NUM_HOTPOINTS ? Color::Blue : color);
            window.draw(shape);
        }

        for (size_t j = 0; j < route.size() - 1; ++j) {
            Vertex line[] = {
                Vertex(Vector2f(points[route[j]].x, points[route[j]].y), color),
                Vertex(Vector2f(points[route[j + 1]].x, points[route[j + 1]].y), color)
            };
            window.draw(line, 2, Lines);
        }
    }

    window.display();
}

int main() {
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Christofides Algorithm Visualization");

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
            // Zorg dat schoolpunt (100, 100) in elke lijst zit
            if (find(selectedPoints[i].begin(), selectedPoints[i].end(), allPoints[0]) == selectedPoints[i].end()) {
                selectedPoints[i].insert(selectedPoints[i].begin(), allPoints[0]);
            }

            sort(selectedPoints[i].begin(), selectedPoints[i].end(), [](const Point& a, const Point& b) {
                return tie(a.x, a.y) < tie(b.x, b.y);
                });
            selectedPoints[i].erase(unique(selectedPoints[i].begin(), selectedPoints[i].end(), [](const Point& a, const Point& b) {
                return a == b && !(a.x == 100 && a.y == 100);
                }), selectedPoints[i].end());

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

    saveRoutesToFile(routes, selectedPoints, allPoints, "routes.txt");

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }
        drawGraph(window, allPoints, routes, selectedPoints);
    }

    return 0;
}
