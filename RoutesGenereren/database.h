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

struct SDTrivia {
    int questionId;
    int correct;
    QString question;
    QString answer1;
    QString answer2;
    QString answer3;

    SDTrivia(int questionId, int correct, const QString &question, const QString &answer1,
        const QString &answer2, const QString &answer3)
        : questionId(questionId), correct(correct), question(question), answer1(answer1),
        answer2(answer2), answer3(answer3) {}
};

class Database {
public:
    Database(const QString &dbName);
    ~Database();

    bool createTables();
    void clearRoutes();
    void insertLocation(const QString &locationName, int x, int y, double cost, int mgId);
    void insertMinigame(int mgType, const QString &mgName, int questionId);
    void insertTrivia(int correct, const QString &question, const QString &answer1, const QString &answer2, const QString &answer3);
    void insertBaseLocations();
    void insertBaseMinigames();
    void insertBaseTrivia();
    QVector<QVector<QVariant>> getLocationsWithMinigames();
    int getRouteAmount();
    std::vector<SDLocations> getRoute(int routeNumber);
    std::vector<SDTrivia> getTrivia();
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
