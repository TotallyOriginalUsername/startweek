// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QSerialPort>
#include <QDir>
#include <QCoreApplication>
#include <QSerialPortInfo>
#include <QStringList>
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlTableModel>

#include <vector>
#include "database.h"
#include "routegenerator.h"

QT_BEGIN_NAMESPACE

class QLabel;
class QTimer;

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);
    void handleBytesWritten(qint64 bytes);
    void handleWriteTimeout();

    void on_btnGenerateRoutes_clicked();
    void on_btnLoadLocationFromDB_clicked();
    void on_btnRefreshRoutes_clicked();

    void on_btnResetAll_clicked();
    void on_btnResetProgress_clicked();
    void on_btnResetScore_clicked();
    void on_btnUploadRoute_clicked();
    void on_btnUploadTime_clicked();
    void on_btnUploadTrivia_clicked();

    void on_btnAddLoc_clicked();
    void on_btnRemoveLoc_clicked();

private:
    void initActionsConnections();
    void showStatusMessage(const QString &message);
    void showWriteError(const QString &message);
    void drawRoutes();
    void refreshRouteList();
    void refreshSerialPorts();
    void loadLocationsFromFile();
    void saveLocationsToFile();
    void initializeDatabase();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    Console *m_console = nullptr;
    SettingsDialog *m_settings = nullptr;
    qint64 m_bytesToWrite = 0;
    QTimer *m_timer = nullptr;
    QSerialPort *m_serial = nullptr;
    QString dataRead;
    QString selectedNode;
    QGraphicsScene *scene = nullptr;
    Database* m_database = nullptr;
    QString m_currentDbPath;
    QSqlTableModel *locationModel;
};

#endif // MAINWINDOW_H
