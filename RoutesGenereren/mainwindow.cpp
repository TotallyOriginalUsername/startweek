#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "routegenerator.h"

#include <QApplication>
#include <QtConcurrent>
#include <QFuture>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>

// Externe data uit routegenerator.cpp
extern std::vector<std::vector<int>> routeIndices;
extern std::vector<std::vector<Point>> selectedPoints;
extern std::vector<Point> allPoints;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialiseer de graphics scene
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Functie om routes visueel weer te geven
void MainWindow::drawRoutes()
{
    scene->clear();  // Leeg het canvas

    const int radius = 4;
    const QColor colors[] = {
        Qt::red, Qt::blue, Qt::green, Qt::darkYellow,
        Qt::magenta, Qt::cyan, Qt::darkGreen, Qt::darkRed,
        Qt::gray, Qt::black
    };

    for (int r = 0; r < routeIndices.size(); ++r) {
        QPen pen(colors[r % 10]);
        pen.setWidth(2);
        const auto& route = routeIndices[r];
        const auto& points = selectedPoints[r];

        // Teken lijnen tussen punten
        for (size_t i = 0; i < route.size() - 1; ++i) {
            const Point& p1 = points[route[i]];
            const Point& p2 = points[route[i + 1]];
            scene->addLine(p1.x, p1.y, p2.x, p2.y, pen);
        }

        // Teken de punten
        for (const auto& pt : points) {
            scene->addEllipse(pt.x - radius, pt.y - radius,
                              radius * 2, radius * 2,
                              QPen(Qt::black), QBrush(colors[r % 10]));
        }
    }

    scene->setSceneRect(scene->itemsBoundingRect());
}

// Bij klikken op de knop: generate en visualiseer
void MainWindow::on_pushButton_clicked()
{
    ui->statusLabel->setText("Bezig met genereren...");
    QApplication::processEvents(); // direct UI updaten

    QFuture<void> future = QtConcurrent::run([this]() {
        generate_routes();  // Genereer de routes

        QMetaObject::invokeMethod(this, [this]() {
            ui->statusLabel->setText("Routes succesvol gegenereerd!");
            drawRoutes();  // Visualiseer routes
        }, Qt::QueuedConnection);
    });
}
