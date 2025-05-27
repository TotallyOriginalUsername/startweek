#include <QApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "routegenerator.h"  // of hoe je bestand heet
#include <QtConcurrent>
#include <QFuture>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    ui->statusLabel->setText("Bezig met genereren...");
    QApplication::processEvents(); // direct UI updaten

    // Draai generate_routes() in aparte thread
    QFuture<void> future = QtConcurrent::run([this]() {
        generate_routes();

        // UI mag niet rechtstreeks uit andere thread: gebruik Qt signal
        QMetaObject::invokeMethod(this, [this]() {
            ui->statusLabel->setText("Routes succesvol gegenereerd!");
        }, Qt::QueuedConnection);
    });
}



