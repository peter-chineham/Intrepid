#include "mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    string argv_str(argv[0]); // Linux + Windows
    MainWindow::base = argv_str.substr(0, argv_str.find("build"));
    MainWindow::base += "intrepid\\Resources\\";

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
