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

#include <chrono>

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

    connect(m_ui->btnRefreshRoutes, &QPushButton::clicked,
            this, &MainWindow::on_btnRefreshRoutes_clicked);
    connect(m_ui->btnRefreshPorts,  &QPushButton::clicked,
            this, &MainWindow::on_btnRefreshPorts_clicked);
    connect(m_ui->btnUploadSerial,  &QPushButton::clicked,
            this, &MainWindow::on_btnUploadSerial_clicked);

    connect(m_ui->btnAddLoc,    &QPushButton::clicked,
            this, &MainWindow::on_btnAddLoc_clicked);
    connect(m_ui->btnEditLoc,   &QPushButton::clicked,
            this, &MainWindow::on_btnEditLoc_clicked);
    connect(m_ui->btnRemoveLoc, &QPushButton::clicked,
            this, &MainWindow::on_btnRemoveLoc_clicked);

    loadLocationsFromFile();
    // if there was nothing in the file, fall back to defaults
    if (allPoints.empty())
        loadDefaultPoints();
    populateLocationsTable();

    refreshRouteList();
    refreshSerialPorts();

    scene = new QGraphicsScene(this);
    m_ui->graphicsView->setScene(scene);
    m_ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    // SerialPort + console instellingen
    //m_console->setEnabled(false);
    //QWidget* tab1 = m_console;
    //m_ui->tabWidget->addTab(tab1, "Developer mode");

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(m_console, &Console::getData, this, &MainWindow::writeData);

    //QObject::connect(m_ui->pushButton_2, SIGNAL(pressed()), this, SLOT(pushButton_2_clicked()));
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

void MainWindow::on_pushButton_clicked()
{
    m_ui->statusLabel->setText("Bezig met genereren...");
    QApplication::processEvents();

    QFuture<void> future = QtConcurrent::run([this]() {
        generate_routes(allPoints);
        QMetaObject::invokeMethod(this, [this]() {
            m_ui->statusLabel->setText("Routes succesvol gegenereerd!");
            drawRoutes();
        }, Qt::QueuedConnection);
    });
}

void MainWindow::pushButton_2_clicked()
{

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
    const QByteArray data = m_serial->readAll();
    m_console->putData(data);
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

void MainWindow::refreshRouteList()
{
    QDir buildDir(QCoreApplication::applicationDirPath());
    buildDir.setNameFilters(QStringList{"*.json"});
    buildDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    auto files = buildDir.entryList();
    m_ui->comboRouteFiles->clear();
    if (files.isEmpty()) {
        m_ui->comboRouteFiles->addItem(tr("[geen routes]"));
    } else {
        m_ui->comboRouteFiles->addItems(files);
    }
}

void MainWindow::refreshSerialPorts()
{
    auto ports = QSerialPortInfo::availablePorts();
    m_ui->comboPorts->clear();
    for (const auto &info : ports)
        m_ui->comboPorts->addItem(info.portName());
    if (ports.isEmpty())
        m_ui->comboPorts->addItem(tr("[geen poorten]"));
}

void MainWindow::on_btnRefreshRoutes_clicked()
{
    refreshRouteList();
}

void MainWindow::on_btnRefreshPorts_clicked()
{
    refreshSerialPorts();
}

void MainWindow::on_btnUploadSerial_clicked()
{
    // Get JSON file
    QString fileName = m_ui->comboRouteFiles->currentText();
    if (fileName.startsWith("[")) {
        QMessageBox::warning(this, tr("Geen route"),
                             tr("Genereer eerst routes op tab 1."));
        return;
    }
    QDir buildDir(QCoreApplication::applicationDirPath());
    QString src = buildDir.filePath(fileName);

    // Get selected port
    QString portName = m_ui->comboPorts->currentText();
    if (portName.startsWith("[")) {
        QMessageBox::warning(this, tr("Geen poort"),
                             tr("Sluit koffer aan en ververs poorten."));
        return;
    }

    // Read JSON
    QFile f(src);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Fout"),
                              tr("Kan %1 niet openen").arg(src));
        return;
    }
    QByteArray jsonData = f.readAll();
    f.close();

    // Open serial port
    QSerialPort port(portName, this);
    port.setBaudRate(QSerialPort::Baud9600);
    port.setDataBits(QSerialPort::Data8);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    if (!port.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Fout"),
                              tr("Kan %1 niet openen:\n%2")
                                .arg(portName, port.errorString()));
        return;
    }

    // Send JSON-route
    port.write(jsonData);
    if (!port.waitForBytesWritten(5000)) {
        QMessageBox::critical(this, tr("Fout"),
                              tr("Schrijven naar %1 timed-out").arg(portName));
        port.close();
        return;
    }

    // Reset progress.txt ("0")
    {
        QByteArray header = "SEND_FILE:progress.txt\n";
        port.write(header);
        port.waitForBytesWritten(100);

        // De werkelijke inhoud
        QByteArray prog = "0";
        port.write(prog);
        port.waitForBytesWritten(100);

        // EOF-markering
        QByteArray footer = "\nEOF\n";
        port.write(footer);
        port.waitForBytesWritten(100);
    }
    
    port.close();
    
    QMessageBox::information(this, tr("Klaar"),
        tr("Route %1 geüpload via %2\nprogress.txt gereset naar 0")
        .arg(fileName, portName));
}

void MainWindow::on_btnAddLoc_clicked()
{
    bool ok = false;
        // 1) Vraag X en Y (int64_t)
    QString xs = QInputDialog::getText(this, tr("Nieuw punt"), tr("X (microdegrees):"), QLineEdit::Normal,
                                       QString(), &ok);
    if (!ok) return;
    int64_t x = xs.toLongLong(&ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Ongeldige invoer"), tr("Geef een geldig getal op."));
        return;
    }

    QString ys = QInputDialog::getText(this, tr("Nieuw punt"), tr("Y (microdegrees):"), QLineEdit::Normal,
                                       QString(), &ok);
    if (!ok) return;
    int64_t y = ys.toLongLong(&ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Ongeldige invoer"), tr("Geef een geldig getal op."));
        return;
    }

        // cost (float)
        double cost = QInputDialog::getDouble(this, tr("Nieuw punt"), tr("Cost:"),
                                              1.0, 0.0, 1e6, 2, &ok);
        if (!ok) return;

        // mg_id (int)
        int mg = QInputDialog::getInt(this, tr("Nieuw punt"), tr("mg_id:"),
                                      0, 0, std::numeric_limits<int>::max(), 1, &ok);
        if (!ok) return;

        allPoints.emplace_back(x, y, static_cast<float>(cost), mg);

        populateLocationsTable();
        saveLocationsToFile();
}

void MainWindow::on_btnEditLoc_clicked()
{
    auto *tbl = m_ui->tableLocations;
    int row = tbl->currentRow();
    if (row < 0 || row >= int(allPoints.size())) {
        QMessageBox::warning(this, tr("Bewerken"), tr("Selecteer eerst een rij."));
        return;
    }

    Point &p = allPoints[row];
    bool ok = false;
    int64_t newX = 0, newY = 0;
    double  newCost = 0.0;
    int     newMg = 0;

    do {
        QString xs = QInputDialog::getText(
            this,
            tr("Bewerk punt"),
            tr("X (microdegrees):"),
            QLineEdit::Normal,
            QString::number(p.x),
            &ok
        );
        if (!ok) return;
        newX = xs.toLongLong(&ok);
        if (!ok) {
            QMessageBox::warning(this,
                tr("Ongeldige invoer"),
                tr("Voer een geldig geheel getal in voor X."));
        }
    } while (!ok);

    do {
        QString ys = QInputDialog::getText(
            this,
            tr("Bewerk punt"),
            tr("Y (microdegrees):"),
            QLineEdit::Normal,
            QString::number(p.y),
            &ok
        );
        if (!ok) return;
        newY = ys.toLongLong(&ok);
        if (!ok) {
            QMessageBox::warning(this,
                tr("Ongeldige invoer"),
                tr("Voer een geldig geheel getal in voor Y."));
        }
    } while (!ok);

    do {
        double c = QInputDialog::getDouble(
            this,
            tr("Bewerk punt"),
            tr("Cost:"),
            double(p.cost),
            0.0, 1e6, 2,
            &ok
        );
        if (!ok) return;
        newCost = c;
    } while (!ok);

    do {
        int m = QInputDialog::getInt(
            this,
            tr("Bewerk punt"),
            tr("mg_id:"),
            p.mg_id,
            0, std::numeric_limits<int>::max(),
            1,
            &ok
        );
        if (!ok) return;
        newMg = m;
    } while (!ok);

    p.x     = newX;
    p.y     = newY;
    p.cost  = static_cast<float>(newCost);
    p.mg_id = newMg;

    populateLocationsTable();
    saveLocationsToFile();
}


void MainWindow::on_btnRemoveLoc_clicked()
{
    auto *tbl = m_ui->tableLocations;
        int row = tbl->currentRow();
        if (row < 0 || row >= int(allPoints.size())) {
            QMessageBox::warning(this, tr("Verwijderen"), tr("Selecteer eerst een rij."));
            return;
        }

        if (QMessageBox::question(this, tr("Verwijderen"),
                tr("Weet je zeker dat je punt %1 wilt verwijderen?")
                 .arg(row+1))
            != QMessageBox::Yes)
        {
            return;
        }

        allPoints.erase(allPoints.begin() + row);
        populateLocationsTable();
        saveLocationsToFile();
}

void MainWindow::loadDefaultPoints()
{
    allPoints = {
        {51688573, 5287210, 2.0f, 0},  // Avans
        {51690224, 5296625, 1.5f, 101},  // Jan de Groot
        {51688460, 5303150, 2.0f, 112},  // Stadhuis
        {51686200, 5304500, 1.5f, 1},  // Sint Jans Kathedraal
        {51685051, 5289156, 2.0f, 9},  // Paleisbrug
        {51684258, 5302611, 1.5f, 111},  // Zuidwal
        {51691299, 5303950, 2.0f, 5},  // Arena
        {51689428, 5310484, 1.5f, 100},  // Nationaal Carnavalsmuseum
        {51695984, 5299074, 2.0f, 113},  // Tramkade
        {51689124, 5303969, 1.5f, 108},  // De Markt
        {51689619, 5299065, 2.0f, 114},  // Bolwerk
        {51693002, 5301264, 1.5f, 115},  // VUE Cinema
        {51697021, 5299328, 2.0f, 102},  // Bossche Brouwers
        {51689724, 5300408, 1.5f, 116},  // Café Bar le Duc
        {51686471, 5304106, 2.0f, 110},  // Museumkwartier
        {51689471, 5303200, 1.5f, 109},  // Moriaan
        {51689302, 5303396, 2.0f, 117},  // ’t Opkikkertje
        {51695457, 5297448, 1.5f, 118},  // Verkadefabriek
        {51694463, 5302862, 2.0f, 119},  // BHIC
        {51690467, 5294925, 1.5f, 8},  // Station
        {51683776, 5317938, 2.0f, 11},  // Zuiderpark
        {51687561, 5305911, 1.5f, 120},  // Korte Putstraat
        {51698630, 5292803, 1.5f, 121},  // Brabanthallen
        {51688691, 5309001, 1.5f, 7},  // Café de Palm
        {51687344, 5305871, 1.5f, 122},  // Bistro Tante Pietje
        {51696264, 5307460, 1.5f, 10},  // Taxandriaplein Park
        {51696501, 5312884, 1.5f, 123},  // IJzeren Vrouw Park
        {51691911, 5286594, 1.5f, 2},  // Simon Stevinweg
        {51690402, 5291740, 1.5f, 124},  // Hugo de Grootplein
        {51685400, 5289354, 1.5f, 3},  // Kinepolis
        {51690484, 5296206, 1.5f, 106},  // Gouden Draak
        {51686618, 5308459, 1.5f, 125},  // Theater aan de Parade
        {51686697, 5303137, 1.5f, 6}   // Gemeentehuis
    };
}

void MainWindow::populateLocationsTable()
{
    auto *tbl = m_ui->tableLocations;
    tbl->clearContents();
    tbl->setRowCount(int(allPoints.size()));
    for (int i = 0; i < int(allPoints.size()); ++i) {
        const Point &p = allPoints[i];
        tbl->setItem(i, 0, new QTableWidgetItem(QString::number(p.x)));
        tbl->setItem(i, 1, new QTableWidgetItem(QString::number(p.y)));
        tbl->setItem(i, 2, new QTableWidgetItem(QString::number(p.cost)));
        tbl->setItem(i, 3, new QTableWidgetItem(QString::number(p.mg_id)));
    }
}

void MainWindow::loadLocationsFromFile()
{
    QString path = QCoreApplication::applicationDirPath() + "/locations.json";

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return;
    auto raw = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isArray())
        return;

    allPoints.clear();
    for (auto v : doc.array()) {
        if (!v.isObject()) continue;
        auto o = v.toObject();
        int64_t x    = qint64(o.value("x").toDouble());
        int64_t y    = qint64(o.value("y").toDouble());
        float   cost = float(o.value("cost").toDouble());
        int     mg   = o.value("mg_id").toInt();
        allPoints.emplace_back(x,y,cost,mg);
    }
}

void MainWindow::saveLocationsToFile()
{
    QString path = QCoreApplication::applicationDirPath() + "/locations.json";


    QJsonArray arr;
    for (auto &p : allPoints) {
        QJsonObject o;
        o["x"]    = double(p.x);
        o["y"]    = double(p.y);
        o["cost"] = double(p.cost);
        o["mg_id"]= p.mg_id;
        arr.append(o);
    }
    QJsonDocument doc(arr);

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("Error"),
            tr("Kan locaties niet opslaan in %1").arg(path));
        return;
    }
    f.write(doc.toJson());
    f.close();
}

