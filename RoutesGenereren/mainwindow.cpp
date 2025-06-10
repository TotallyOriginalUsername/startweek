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

    // Initialiseer QGraphicsScene
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

    QObject::connect(m_ui->pushButton_2, SIGNAL(pressed()), this, SLOT(pushButton_2_clicked()));
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

void MainWindow::drawRoutes()
{
    scene->clear();

    // 1) Bepaal min/max van alle getoonde punten
    int64_t minX = std::numeric_limits<int64_t>::max(), maxX = std::numeric_limits<int64_t>::lowest();
    int64_t minY = std::numeric_limits<int64_t>::max(), maxY = std::numeric_limits<int64_t>::lowest();
    for (auto& pts : selectedPoints)
        for (auto& p : pts) {
            minX = std::min(minX, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
        }

    // 2) Bereken schaal (+ marge)
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

    // 3) Teken elke route in een andere kleur
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
        generate_routes();
        QMetaObject::invokeMethod(this, [this]() {
            m_ui->statusLabel->setText("Routes succesvol gegenereerd!");
            drawRoutes();
        }, Qt::QueuedConnection);
    });
}

void MainWindow::pushButton_2_clicked()
{
    QByteArray dataToSend("help \r\n");
    writeData(dataToSend);
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
