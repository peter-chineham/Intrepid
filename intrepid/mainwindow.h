#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <iostream>
#include <math.h>
#include <QMainWindow>
#include <QDialog>
#include <QtGui>
#include <QtCore>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QString>
#include <windows.h> // Make cout's appear in Visual Studio!
#include "Reader2.h"

class Canvas;

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static string base;

private slots:
    void on_actionRead_Input_File_triggered();

    void on_actionMentor_triggered();

    void on_actionMentMUX_triggered();

    void on_actionAll_triggered();

    void on_actionBackbone_triggered();

    void on_actionCluster_triggered();

    void on_actionTree_triggered();

    void on_actionDirect_triggered();

    void on_actionAll_2_triggered();

    void on_actionBackbone_2_triggered();

    void on_actionNone_triggered();

    void on_actionNormal_triggered();

    void on_actionUtilisation_triggered();

    void on_actionStandard_triggered();

    void on_actionCSV_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    Canvas *canvas;

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
};

#endif // MAINWINDOW_H
