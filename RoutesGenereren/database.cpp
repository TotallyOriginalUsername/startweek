#include "database.h"
#include <QDebug>
#include <QSqlRecord>
#include <qdebug.h>
#include <qglobal.h>
#include <qsqlquery.h>
#include <QMessageBox>
#include <vector>

Database::Database(const QString &dbName) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "Error: Unable to open database" << db.lastError().text();
    }
}

Database::~Database() {
    db.close();
}

bool Database::createTables() {
    QSqlQuery query;
    bool success = true;

    success &= query.exec("CREATE TABLE IF NOT EXISTS Minigames ("
                          "MG_ID INTEGER, "
                          "MG_TYPE INTEGER, "
                          "MG_name TEXT, "
                          "Question_ID INTEGER, "
                          "PRIMARY KEY(MG_ID))");

    success &= query.exec("CREATE TABLE IF NOT EXISTS Locations ("
                          "LOC_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "Location_Name TEXT, "
                          "X INTEGER, "
                          "Y INTEGER, "
                          "Cost REAL, "
                          "MG_ID INTEGER, "
                          "FOREIGN KEY(MG_ID) REFERENCES Minigames(MG_ID))");

    success &= query.exec("CREATE TABLE IF NOT EXISTS Routes ("
                      "Route_ID INTEGER, "
                      "Position INTEGER, "
                      "LOC_ID INTEGER, "
                      "PRIMARY KEY(Route_ID, Position), "
                      "FOREIGN KEY(Route_ID) REFERENCES Routes(Route_ID), "
                      "FOREIGN KEY(LOC_ID) REFERENCES Locations(LOC_ID))");

    return success;
}

void Database::clearRoutes(){
    QSqlQuery query;
    query.exec("DELETE FROM Routes");
}

void Database::insertLocation(const QString &locationName, int x, int y, double cost, int mgId) {
    QSqlQuery query;
    query.prepare("INSERT INTO Locations (Location_Name, X, Y, Cost, MG_ID) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(locationName);
    query.addBindValue(x);
    query.addBindValue(y);
    query.addBindValue(cost);
    query.addBindValue(mgId);
    if (!query.exec()) {
        qWarning() << "Failed to insert location:" << query.lastError().text();
    }
}

void Database::insertMinigame(int mgId, int mgType, const QString &mgName, int questionId) {
    QSqlQuery query;
    query.prepare("INSERT INTO Minigames (MG_ID, MG_TYPE, MG_name, Question_ID) VALUES (?, ?, ?, ?)");
    query.addBindValue(mgId);
    query.addBindValue(mgType);
    query.addBindValue(mgName);
    query.addBindValue(questionId);
    if (!query.exec()) {
        qWarning() << "Failed to insert minigame:" << query.lastError().text();
    }
}

void Database::insertBaseLocations(){
    insertLocation("Avans", 51688573, 5287210, 2.0, 0);
    insertLocation("Jan de Groot", 51690224, 5296625, 1.5, 1);
    insertLocation("Stadhuis", 51688460, 5303150, 2.0, 2);
    insertLocation("Sint Jans Kathedraal", 51686200, 5304500, 1.5, 3);
    insertLocation("Paleisbrug", 51685051, 5289156, 2.0, 4);
    insertLocation("Zuidwal", 51684258, 5302611, 1.5, 5);
    insertLocation("Arena", 51691299, 5303950, 2.0, 6);
    insertLocation("Nationaal Carnavalsmuseum", 51689428, 5310484, 1.5, 7);
    insertLocation("Tramkade", 51695984, 5299074, 2.0, 8);
    insertLocation("De Markt", 51689124, 5303969, 1.5, 9);
    insertLocation("Bolwerk", 51689619, 5299065, 2.0, 10);
    insertLocation("VUE Cinema", 51693002, 5301264, 1.5, 11);
    insertLocation("Bossche Brouwers", 51697021, 5299328, 2.0, 12);
    insertLocation("Café Bar le Duc", 51689724, 5300408, 1.5, 13);
    insertLocation("Museumkwartier", 51686471, 5304106, 2.0, 14);
    insertLocation("Moriaan", 51689471, 5303200, 1.5, 15);
    insertLocation("’t Opkikkertje", 51689302, 5303396, 2.0, 16);
    insertLocation("Verkadefabriek", 51695457, 5297448, 1.5, 17);
    insertLocation("BHIC", 51694463, 5302862, 2.0, 18);
    insertLocation("Station", 51690467, 5294925, 1.5, 19);
    insertLocation("Zuiderpark", 51683776, 5317938, 2.0, 20);
    insertLocation("Korte Putstraat", 51687561, 5305911, 1.5, 21);
    insertLocation("Brabanthallen", 51698630, 5292803, 1.5, 22);
    insertLocation("Café de Palm", 51688691, 5309001, 1.5, 23);
    insertLocation("Bistro Tante Pietje", 51687344, 5305871, 1.5, 0);
    insertLocation("Taxandriaplein Park", 51696264, 5307460, 1.5, 0);
    insertLocation("IJzeren Vrouw Park", 51696501, 5312884, 1.5, 0);
    insertLocation("Simon Stevinweg", 51691911, 5286594, 1.5, 0);
    insertLocation("Hugo de Grootplein", 51690402, 5291740, 1.5, 0);
    insertLocation("Kinepolis", 51685400, 5289354, 1.5, 0);
    insertLocation("Gouden Draak", 51690484, 5296206, 1.5, 0);
    insertLocation("Theater aan de Parade", 51686618, 5308459, 1.5, 0);
    insertLocation("Gemeentehuis", 51686697, 5303137, 1.5, 0);
}

void Database::insertBaseMinigames(){
    insertMinigame(0, 0, "trivia vraag 1", 0);
    insertMinigame(1, 0, "trivia vraag 2", 1);
    insertMinigame(2, 0, "trivia vraag 3", 2);
    insertMinigame(3, 0, "trivia vraag 4", 3);
    insertMinigame(4, 0, "trivia vraag 5", 4);
    insertMinigame(5, 0, "trivia vraag 6", 5);
    insertMinigame(6, 0, "trivia vraag 7", 6);
    insertMinigame(7, 0, "trivia vraag 8", 7);
    insertMinigame(8, 0, "trivia vraag 9", 8);
    insertMinigame(9, 0, "trivia vraag 10", 9);
    insertMinigame(10, 0, "trivia vraag 11", 10);
    insertMinigame(11, 0, "trivia vraag 12", 11);
    insertMinigame(12, 1, "Snake", 0);
    insertMinigame(13, 2, "Simon Says", 0);
    insertMinigame(14, 3, "Kopieer de vorm", 0);
    insertMinigame(15, 4, "Pokemon", 0);
    insertMinigame(16, 5, "Vang de rode ballen", 0);
    insertMinigame(17, 6, "Timer minigame", 0);
    insertMinigame(18, 7, "Volg de rode lijn", 0);
    insertMinigame(19, 8, "10 Seconden", 0);
    insertMinigame(20, 9, "Hoger of lager", 0);
    insertMinigame(21, 10, "Whack a Mole", 0);
    insertMinigame(22, 11, "Space invaders - knoppen", 0);
    insertMinigame(23, 12, "Space invaders - kantel", 0);
}

QVector<QVector<QVariant>> Database::getLocationsWithMinigames() {
    QVector<QVector<QVariant>> results;
    QSqlQuery query("SELECT Locations.Location_Name, Locations.X, Locations.Y, Locations.Cost, Minigames.MG_ID, Minigames.MG_name "
                    "FROM Locations "
                    "LEFT JOIN Minigames ON Locations.MG_ID = Minigames.MG_ID");

    while (query.next()) {
        QVector<QVariant> row;
        for (int i = 0; i < query.record().count(); ++i) {
            row.append(query.value(i));
        }
        results.append(row);
    }

    return results;
}

int Database::getRouteAmount(){
    int result = 0;
    QSqlQuery query("SELECT COUNT(DISTINCT Route_ID) FROM Routes");

    while (query.next()) {
        result = query.value(0).toInt();
    }
    return result;
}

std::vector<SDLocations> Database::getRoute(int routeNumber){
    std::vector<SDLocations> results;
    QSqlQuery query;
    query.prepare("SELECT X, Y, Minigames.MG_TYPE, Minigames.Question_ID FROM Routes "
                        "LEFT JOIN Locations on Routes.LOC_ID = Locations.LOC_ID "
                        "LEFT JOIN Minigames on Locations.MG_ID = Minigames.MG_ID "
                        "WHERE Route_ID = ? "
                        "ORDER BY Position");
    query.addBindValue(routeNumber);

    if (!query.exec()) {
        qWarning() << "Failed to retrieve route:" << query.lastError().text();
    }

    while (query.next()) {
        int x = query.value(0).toInt();
        int y = query.value(1).toInt();
        int mgType = query.value(2).toInt();
        int questionId = query.value(3).toInt();

        qDebug() << "X:" << x << "Y:" << y << "MG ID:" << mgType << "Tri ID:" << questionId;

        //results.emplace(results.begin(),query.value(0).toInt(), 
        //query.value(1).toInt(),query.value(2).toInt(),query.value(3).toInt());
        results.emplace_back(x, y, mgType, questionId);
    }
        
    return results;
}

void Database::setJsonPoints(std::vector<Point>& inputPoints) {
    QSqlQuery query("SELECT LOC_ID, X, Y, Cost, MG_ID FROM Locations");

    if (!query.exec()) {
        qWarning() << "Failed to retrieve locations:" << query.lastError().text();
        return;
    }

    inputPoints.clear();
    while (query.next()) {
        int locId = query.value(0).toInt();
        int64_t x = query.value(1).toInt();
        int64_t y = query.value(2).toInt();
        float cost = query.value(3).toFloat();
        int mgId = query.value(4).toInt();
        inputPoints.emplace_back(locId, x, y, cost, mgId);
    }
    qInfo("a");
}

void Database::setAllPoints(std::vector<std::tuple<int, int, double, int>>& allPoints) {
    QSqlQuery query("SELECT X, Y, Cost, MG_ID FROM Locations");

    if (!query.exec()) {
        qWarning() << "Failed to retrieve locations:" << query.lastError().text();
        return;
    }

    allPoints.clear();
    while (query.next()) {
        int x = query.value(0).toInt();
        int y = query.value(1).toInt();
        double cost = query.value(2).toDouble();
        int mgId = query.value(3).toInt();
        allPoints.emplace_back(x, y, cost, mgId);
    }
}

void Database::setRoutes(const std::vector<std::vector<int>>& routes)
{
    int routeId = 1;

    for(const auto& route : routes)
    {
        for(int i = 0; i < route.size(); i++)
        {
            QSqlQuery insert;

            insert.prepare(
                "INSERT INTO Routes(Route_ID, Position, LOC_ID)"
                "VALUES(?,?,?)"
            );

            insert.addBindValue(routeId);
            insert.addBindValue(i);
            insert.addBindValue(route[i]);

            insert.exec();
        }
        routeId++;
    }
}

void Database::printDatabase() {
    QSqlQuery query("SELECT * FROM Locations");
    qDebug() << "Locations Table:";
    while (query.next()) {
        qDebug() << query.value(0).toString() << query.value(1).toInt() << query.value(2).toInt() << query.value(3).toDouble() << query.value(4).toInt();
    }

    query.exec("SELECT * FROM Minigames");
    qDebug() << "Minigames Table:";
    while (query.next()) {
        qDebug() << query.value(0).toInt() << query.value(1).toString() << query.value(2).toInt();
    }
}

void Database::clearDatabase() {
    QSqlQuery query;
    query.exec("DELETE FROM Locations");
    query.exec("DELETE FROM Minigames");
}

void Database::resetDatabase(){
    qInfo("Resetting  db...");
    QSqlQuery query;
    if (query.exec("SELECT name FROM sqlite_master WHERE type='table'")) {
        while (query.next()) {
            QString tableName = query.value(0).toString();
            QString dropTable = QString("DROP TABLE IF EXISTS %1").arg(tableName);
            query.exec(dropTable);
        }
    }

    createTables();
    insertBaseLocations();
    insertBaseMinigames();
}

// 0 upon sucess, 1 upon no locations, 2 upon no minigames, 3 if database lacks the tables
int Database::validateDatabase(){
    QSqlQuery query;
    if (query.exec("SELECT COUNT(*) FROM Locations")){
        query.next();
        if(query.value(0).toInt() <= 0){
            qInfo("Lack of loc info");
            return 1;
        }
    }
    else {
        qInfo("Database has no locations table");
        return 3;
    }
    if (query.exec("SELECT COUNT(*) FROM Minigames")){
        query.next();
        if(query.value(0).toInt() <= 0){
            qInfo("Lack of mg info");
            return 2;
        }
    }
    else {
        qInfo("Database has no minigames");
        return 3;
    }

    return 0;
}
