#include "mainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    string argv_str(argv[0]); // Linux + Windows
    MainWindow::base = argv_str.substr(0, argv_str.find("\\intrepid\\intrepid\\") + 19);
    MainWindow::base += "Resources\\";

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
