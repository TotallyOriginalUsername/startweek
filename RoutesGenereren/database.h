#ifndef DATABASE_H
#define DATABASE_H

#include "routegenerator.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QString>
#include <QVariant>
#include <vector>


struct SDLocations {
    int x;
    int y;
    int mg_type;
    int trivia_id;

    SDLocations(int64_t x, int64_t y, int mg, int trivia)
        : x(x), y(y), mg_type(mg), trivia_id(trivia) {}
};

class Database {
public:
    Database(const QString &dbName);
    ~Database();

    bool createTables();
    void clearRoutes();
    void insertLocation(const QString &locationName, int x, int y, double cost, int mgId);
    void insertMinigame(int mgId, int mgType, const QString &mgName, int questionId);
    void insertBaseLocations();
    void insertBaseMinigames();
    QVector<QVector<QVariant>> getLocationsWithMinigames();
    int getRouteAmount();
    std::vector<SDLocations> getRoute(int routeNumber);
    void setJsonPoints(std::vector<Point>& inputPoints);
    void setAllPoints(std::vector<std::tuple<int, int, double, int>>& allPoints);
    void setRoutes(const std::vector<std::vector<int>>& routes);

    void printDatabase();
    void clearDatabase();
    void resetDatabase();
    int validateDatabase();

private:
    QSqlDatabase db;
};

#endif // DATABASE_H
