#include "database.h"
#include <QDebug>
#include <QSqlRecord>

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

    success &= query.exec("CREATE TABLE IF NOT EXISTS Locations ("
                          "Location_Name TEXT, "
                          "X INTEGER, "
                          "Y INTEGER, "
                          "Cost REAL, "
                          "MG_ID INTEGER)");

    success &= query.exec("CREATE TABLE IF NOT EXISTS Minigames ("
                          "MG_ID INTEGER, "
                          "MG_name TEXT, "
                          "Question_ID INTEGER)");

    return success;
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

void Database::insertMinigame(int mgId, const QString &mgName, int questionId) {
    QSqlQuery query;
    query.prepare("INSERT INTO Minigames (MG_ID, MG_name, Question_ID) VALUES (?, ?, ?)");
    query.addBindValue(mgId);
    query.addBindValue(mgName);
    query.addBindValue(questionId);
    if (!query.exec()) {
        qWarning() << "Failed to insert minigame:" << query.lastError().text();
    }
}

void Database::insertBaseLocations(){
    insertLocation("Avans", 51688573, 5287210, 2.0, 0);
    insertLocation("Jan de Groot", 51690224, 5296625, 1.5, 101);
    insertLocation("Stadhuis", 51688460, 5303150, 2.0, 112);
    insertLocation("Sint Jans Kathedraal", 51686200, 5304500, 1.5, 1);
    insertLocation("Paleisbrug", 51685051, 5289156, 2.0, 9);
    insertLocation("Zuidwal", 51684258, 5302611, 1.5, 111);
    insertLocation("Arena", 51691299, 5303950, 2.0, 5);
    insertLocation("Nationaal Carnavalsmuseum", 51689428, 5310484, 1.5, 100);
    insertLocation("Tramkade", 51695984, 5299074, 2.0, 113);
    insertLocation("De Markt", 51689124, 5303969, 1.5, 108);
    insertLocation("Bolwerk", 51689619, 5299065, 2.0, 114);
    insertLocation("VUE Cinema", 51693002, 5301264, 1.5, 115);
    insertLocation("Bossche Brouwers", 51697021, 5299328, 2.0, 102);
    insertLocation("Café Bar le Duc", 51689724, 5300408, 1.5, 116);
    insertLocation("Museumkwartier", 51686471, 5304106, 2.0, 110);
    insertLocation("Moriaan", 51689471, 5303200, 1.5, 109);
    insertLocation("’t Opkikkertje", 51689302, 5303396, 2.0, 117);
    insertLocation("Verkadefabriek", 51695457, 5297448, 1.5, 118);
    insertLocation("BHIC", 51694463, 5302862, 2.0, 119);
    insertLocation("Station", 51690467, 5294925, 1.5, 8);
    insertLocation("Zuiderpark", 51683776, 5317938, 2.0, 11);
    insertLocation("Korte Putstraat", 51687561, 5305911, 1.5, 120);
    insertLocation("Brabanthallen", 51698630, 5292803, 1.5, 121);
    insertLocation("Café de Palm", 51688691, 5309001, 1.5, 7);
    insertLocation("Bistro Tante Pietje", 51687344, 5305871, 1.5, 122);
    insertLocation("Taxandriaplein Park", 51696264, 5307460, 1.5, 10);
    insertLocation("IJzeren Vrouw Park", 51696501, 5312884, 1.5, 123);
    insertLocation("Simon Stevinweg", 51691911, 5286594, 1.5, 2);
    insertLocation("Hugo de Grootplein", 51690402, 5291740, 1.5, 124);
    insertLocation("Kinepolis", 51685400, 5289354, 1.5, 3);
    insertLocation("Gouden Draak", 51690484, 5296206, 1.5, 106);
    insertLocation("Theater aan de Parade", 51686618, 5308459, 1.5, 125);
    insertLocation("Gemeentehuis", 51686697, 5303137, 1.5, 6);
}

void Database::insertBaseMinigames(){
    insertMinigame(0, "trivia vraag 1", 0);
    insertMinigame(0, "trivia vraag 2", 1);
    insertMinigame(0, "trivia vraag 3", 2);
    insertMinigame(1, "Snake", 0);
    insertMinigame(2, "Simon Says", 0);
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
