#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << QStyleFactory::keys();
    QPalette p;

    p.setColor(QPalette::Window, QColor(193,193,193));
    p.setColor(QPalette::WindowText, Qt::black);
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::AlternateBase, QColor(193,193,193));

    p.setColor(QPalette::Button, QColor(193,193,193));
    p.setColor(QPalette::ButtonText, Qt::black);

    p.setColor(QPalette::Highlight, QColor(0,0,127));
    p.setColor(QPalette::HighlightedText, Qt::white);

    a.setPalette(p);
    a.setStyle(QStyleFactory::create("Windows"));
    a.setFont(QFont("MS Sans Serif", 8));

    MainWindow w;
    w.show();
    return a.exec();
}
