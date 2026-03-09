#ifndef DATABASE_H
#define DATABASE_H

#include "routegenerator.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QString>
#include <QVariant>

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
