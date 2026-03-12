// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "routegenerator.h"
#include "settingsdialog.h"

#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QtConcurrent>
#include <QFuture>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPainter>
#include <limits>
#include <QPen>
#include <QBrush>
#include <QInputDialog>
#include <QFileDialog>

#include <chrono>
#include <qchar.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qglobal.h>

static constexpr std::chrono::seconds kWriteTimeout = std::chrono::seconds{5};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_console(new Console),
    m_settings(new SettingsDialog(this)),
    m_serial(new QSerialPort(this))
{
    m_ui->setupUi(this);

    m_ui->tabWidget->setCurrentIndex(0);

    initializeDatabase();
    m_database->setJsonPoints(allPoints);

    scene = new QGraphicsScene(this);
    m_ui->graphicsView->setScene(scene);
    m_ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    // SerialPort + console instellingen
    m_console->setEnabled(false);
    QWidget* tab1 = m_console;
    m_ui->tabWidget->addTab(tab1, "Developer mode");

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(m_console, &Console::getData, this, &MainWindow::writeData);
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

void MainWindow::drawRoutes()
{
    scene->clear();

    int64_t minX = std::numeric_limits<int64_t>::max(), maxX = std::numeric_limits<int64_t>::lowest();
    int64_t minY = std::numeric_limits<int64_t>::max(), maxY = std::numeric_limits<int64_t>::lowest();
    for (auto& pts : selectedPoints)
        for (auto& p : pts) {
            minX = std::min(minX, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
        }

    QSizeF viewSz = m_ui->graphicsView->viewport()->size();
    constexpr qreal M = 20.0;  // marge in pixels
    qreal scaleX = (viewSz.width()  - 2*M) / qreal(maxX - minX);
    qreal scaleY = (viewSz.height() - 2*M) / qreal(maxY - minY);
    qreal scale = std::min(scaleX, scaleY);

    auto mapPt = [&](const Point& p) {
        qreal x = (p.x - minX)*scale + M;
        qreal y = (p.y - minY)*scale + M;
        return QPointF(x,y);
    };

    const int radius = 4;
    const QColor colors[] = {
        Qt::red, Qt::blue, Qt::green, Qt::darkYellow,
        Qt::magenta, Qt::cyan, Qt::darkGreen, Qt::darkRed,
        Qt::gray, Qt::black
    };

    for (int r = 0; r < routeIndices.size(); ++r) {
        QPen pen(colors[r%10]);
        pen.setCosmetic(true);
        pen.setWidth(0);
        const auto& route  = routeIndices[r];
        const auto& pts    = selectedPoints[r];

        // lijnen
        for (int i = 0; i + 1 < route.size(); ++i) {
            QPointF p1 = mapPt(pts[ route[i]   ]);
            QPointF p2 = mapPt(pts[ route[i+1] ]);
            scene->addLine(p1.x(), p1.y(), p2.x(), p2.y(), pen);
        }
        // punten
        for (auto& p : pts) {
            QPointF c = mapPt(p);
            scene->addEllipse(c.x()-radius, c.y()-radius,
                              radius*2, radius*2,
                              QPen(Qt::black),
                              QBrush(colors[r%10]));
        }
    }

    // 4) Scene instellen en fitten
    scene->setSceneRect(0, 0, viewSz.width(), viewSz.height());
    m_ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    if (scene && !scene->items().isEmpty()) {
        m_ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::on_btnGenerateRoutes_clicked()
{
    std::vector<std::vector<int>> routes;
    if(allPoints.empty()){
        QMessageBox::warning(this, "Error", "Geen locaties ingeladen!\n"
            "Laad eerst de locaties in");
            return;
    }
    m_ui->statusLabel->setText("Bezig met genereren...");
    QApplication::processEvents();

    m_database->clearRoutes();
    generate_routes(allPoints, routes);
    m_database->setRoutes(routes);
    QMetaObject::invokeMethod(this, [this]() {
        m_ui->statusLabel->setText("Routes succesvol gegenereerd!");
        drawRoutes();
    }, Qt::QueuedConnection);
    on_btnRefreshRoutes_clicked();
}

void MainWindow::on_btnLoadLocationFromDB_clicked(){
    qInfo("Loading locations from (updated) db");
    m_database->setJsonPoints(allPoints);
}

void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                              .arg(p.name, p.stringBaudRate, p.stringDataBits,
                                   p.stringParity, p.stringStopBits, p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        showStatusMessage(tr("Open error"));
    }
    // It needs some encouragement in order to work properly
    writeData("Ganbatte\r\n");
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
    m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Serial Terminal"),
                       tr("The <b>Serial Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}

void MainWindow::readData()
{
    QRegularExpression numberFilter("(\\w+):\\s*([0-9]+)");
    while (m_serial->canReadLine())
    {
        QByteArray line = m_serial->readLine();
        QString trimmedline = QString::fromUtf8(line).trimmed();

        m_console->putData(line);
        trimmedline.remove("\u001B[1;32muart:~$ \u001B[m");
        trimmedline.remove("\u001B[8D\u001B[J");

        QRegularExpressionMatch match = numberFilter.match(trimmedline);
        
        if (match.hasMatch())
        {
            QString label = match.captured(1);
            QString number = match.captured(2);

            if (label == "Score")
            {
                m_ui->lblScore->setText(number);
            }

            if (label == "Progress")
            {
                m_ui->lblProgress->setText(number);
            }
        }

        qDebug() << "Received:" << trimmedline;
    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::handleBytesWritten(qint64 bytes)
{
    m_bytesToWrite -= bytes;
    if (m_bytesToWrite == 0)
        m_timer->stop();
}

void MainWindow::handleWriteTimeout()
{
    const QString error = tr("Write operation timed out for port %1.\n"
                             "Error: %2").arg(m_serial->portName(),
                                   m_serial->errorString());
    showWriteError(error);
}

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_console, &Console::clear);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void MainWindow::showWriteError(const QString &message)
{
    QMessageBox::warning(this, tr("Warning"), message);
}

void MainWindow::on_btnRefreshRoutes_clicked()
{
    int routeAmount = m_database->getRouteAmount();

    m_ui->comboRouteFiles->clear();
    if(routeAmount == 0){
        m_ui->comboRouteFiles->addItem(tr("[geen routes]"));
    } else{
        for (int i = 1; i <= routeAmount; i++) {
            m_ui->comboRouteFiles->addItem(QString::number(i));
        }
    }
}

void MainWindow::on_btnResetAll_clicked()
{
    QByteArray dataToSend("sd reset all\r\n");
    writeData(dataToSend);
}

void MainWindow::on_btnResetScore_clicked()
{
    QByteArray dataToSend("sd set score 0\r\n");
    writeData(dataToSend);
}

void MainWindow::on_btnResetProgress_clicked()
{
    QByteArray dataToSend("sd set progress 0\r\n");
    writeData(dataToSend);
}

void MainWindow::on_btnUploadRoute_clicked()
{
    QString routeNumber2 = m_ui->comboRouteFiles->currentText();
    std::vector<SDLocations> route2 = m_database->getRoute(routeNumber2.toInt());

    if(m_serial->isOpen()){
    QString routeNumber = m_ui->comboRouteFiles->currentText();
    std::vector<SDLocations> route = m_database->getRoute(routeNumber.toInt());
    int locationAmount = route.size();
    QByteArray start("loc start\r\n");
    QByteArray save("loc save\r\n");
    QByteArray reload("loc reload\r\n");
    writeData(start);
    // The hardware's RX ring buffer will be too full if you try to send every location without delays.
    // waitForBytesWritten doesnt account for the wait on hw side, only for the pc to send everything
    m_serial->waitForBytesWritten(4000);
    QThread::msleep(300);

    for(int i = 0; i < locationAmount; i++){
        QByteArray locAdd = QString("loc add %1 %2 %3 %4\r\n")
            .arg(route[i].x)
            .arg(route[i].y)
            .arg(route[i].mg_type)
            .arg(route[i].trivia_id)
            .toUtf8();
            //qDebug() << locAdd;

        writeData(locAdd);
        m_serial->waitForBytesWritten(4000);
        QThread::msleep(300);
    }
    writeData(save);
    m_serial->waitForBytesWritten(4000);
    QThread::msleep(300);
    writeData(reload);

    QMessageBox::information(this, tr("Klaar"),
        tr("%1 locaties geuploadt naar de koffer")
        .arg(locationAmount));
    }
    else {
        QMessageBox::warning(this, tr("Error!"), tr("Open eerst een verbinding met de koffer"));
    }

}

void MainWindow::on_btnUploadTime_clicked(){
    QTime start = m_ui->timeEdit_start->time();
    QTime end = m_ui->timeEdit_end->time();
    if ( start >= end){
        QMessageBox::warning(this, tr("Error"), tr("Starttijd moet voor eindtijd zijn"));
        return;
    }
    int sd_start = (start.hour() * 60) - 120 + start.minute();
    int sd_end = (end.hour() * 60) - 120 + end.minute();
    qInfo("Start: %d, End: %d", sd_start, sd_end);

    QByteArray startData = QString("sd set starttime %1\r\n")
                .arg(sd_start)
                .toUtf8();

    writeData(startData);
    
    QByteArray endData = QString("sd set endtime %1\r\n")
                .arg(sd_end)
                .toUtf8();

    writeData(endData);
}

void MainWindow::on_btnUploadTrivia_clicked(){
    qDebug() << "Not implemented yet";
}

void MainWindow::on_btnReadScore_clicked()
{
    QByteArray dataToSend("sd get score\r\n");
    writeData(dataToSend);
}

void MainWindow::on_btnReadProgress_clicked()
{
    QByteArray dataToSend("sd get progress\r\n");
    writeData(dataToSend);
}

void MainWindow::on_btnTestData_clicked(){
    QByteArray dataToSend("sd set progress 10\r\n");
    writeData(dataToSend);
    dataToSend = "sd set score 100\r\n";
    writeData(dataToSend);
}


void MainWindow::on_btnGetLoc_clicked(){
    QByteArray dataToSend("loc get\r\n");
    writeData(dataToSend);
}

void MainWindow::on_btnAddLoc_clicked()
{
    bool ok = false;

    QString name = QInputDialog::getText(this, "Nieuwe locatie",
                                         "Naam:", QLineEdit::Normal,
                                         "", &ok);
    if (!ok || name.isEmpty()){
        QMessageBox::warning(this, tr("Ongeldige invoer"), tr("Geef een geldige naam op."));
        return;
    }

    int x = QInputDialog::getInt(this, "Nieuwe locatie",
                                 "X coordinaat:", 0,
                                 0, 1000000, 1, &ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Ongeldige invoer"), tr("Geef een geldig getal op."));
        return;
    }

    int y = QInputDialog::getInt(this, "Nieuwe locatie",
                                 "Y coordinaat:", 0,
                                 0, 1000000000, 1, &ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Ongeldige invoer"), tr("Geef een geldig getal op."));
        return;
    }

    double cost = QInputDialog::getDouble(this, "Nieuwe locatie",
                                          "Cost:", 1.0,
                                          0.0, 10.0, 2, &ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Ongeldige invoer"), tr("Geef een geldig getal op."));
        return;
    }

    int mgId = QInputDialog::getInt(this, "Nieuwe locatie",
                                    "Minigame ID:", 0,
                                    0, 250, 1, &ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Ongeldige invoer"), tr("Geef een geldig getal op."));
        return;
    }

    int row = locationModel->rowCount();
    locationModel->insertRow(row);

    locationModel->setData(locationModel->index(row, 0), name);
    locationModel->setData(locationModel->index(row, 1), x);
    locationModel->setData(locationModel->index(row, 2), y);
    locationModel->setData(locationModel->index(row, 3), cost);
    locationModel->setData(locationModel->index(row, 4), mgId);

    locationModel->submitAll();
}

void MainWindow::on_btnRemoveLoc_clicked()
{
    auto *view = m_ui->tableLocations;
    QModelIndex index = view->currentIndex();
    if (!index.isValid()){
        QMessageBox::warning(this, tr("Verwijderen"), tr("Selecteer eerst een rij."));
            return;
    }

    locationModel->removeRow(index.row());
    locationModel->submitAll();
    // refresh the model, otherwise an empty ! row remains
    locationModel->select();
}

void MainWindow::initializeDatabase(){
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open een bestaande database"),
        "",
        tr("Database Files (*.db *.sqlite)")
    );

    QString path;
    if(!fileName.isEmpty()){
        qInfo("Using user db");
        path = fileName;
    }
    else {
        qInfo("Using default db");
        path = QCoreApplication::applicationDirPath() + "/locations.db";
    }

    m_database = new Database(path);
    int ret = m_database->validateDatabase();
    switch (ret) {
        case 0:
            break;
        case 1:
            QMessageBox::warning(this, "Error", "Database heeft geen locaties\n"
            "Backup locaties worden gebruikt");
            m_database->insertBaseLocations();
            break;
        case 2:
            QMessageBox::warning(this, "Error", "Database heeft geen minigames\n"
            "Backup minigames worden gebruikt");
            m_database->insertBaseMinigames();
            break;
        case 3:
            QMessageBox::warning(this, "Error", "Database heeft geen juiste tabellen\n");
            m_database->resetDatabase();
            break;
        default:
            break;
    }

    locationModel = new QSqlRelationalTableModel(this);
    locationModel->setTable("Locations");
    locationModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    locationModel->setRelation(locationModel->fieldIndex("MG_ID"),
                               QSqlRelation("Minigames", "MG_ID", "MG_name"));
    locationModel->select();

    m_ui->tableLocations->setModel(locationModel);
    m_ui->tableLocations->setItemDelegate(new QSqlRelationalDelegate(m_ui->tableLocations));
    m_ui->tableLocations->setColumnHidden(locationModel->fieldIndex("LOC_ID"), true);
    m_ui->tableLocations->resizeColumnsToContents();
    m_ui->tableLocations->horizontalHeader()->setStretchLastSection(true);
}

