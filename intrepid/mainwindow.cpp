#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "canvas.h"
#include "Dictionary.h"
#include "mentor.h"
#include "mentmux.h"
#include "Reader2.h"
#include "Utils.h"

string MainWindow::base = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Make cout's appear in Visual Studio!
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
    cout << base << endl;

    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    canvas = new Canvas();
    scene->addItem(canvas);
    Canvas::setCanvas(canvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
//    cout << "resizeEvent Event width:" << event->size().width() << " Event height:" << event->size().height() << endl;
    canvas->pv_width = ui->graphicsView->width()-3;
    canvas->pv_height = ui->graphicsView->height()-5;
    scene->setSceneRect(QRectF(0, 0, canvas->pv_width, canvas->pv_height));
    canvas->zoomExtents();
//    cout << "resizeEvent canvas width:"  << canvas->pv_width << "       height:" << canvas->pv_height << endl;
//    cout << "resizeEvent  scene width: " << scene->width()   << " scene height:" << scene->height()   << endl;
}

void MainWindow::showEvent(QShowEvent *event){
    canvas->pv_width  = ui->graphicsView->width()-3;
    canvas->pv_height = ui->graphicsView->height()-5;
    scene->setSceneRect(QRectF(0, 0, canvas->pv_width, canvas->pv_height));
    canvas->zoomExtents();
//    cout << "showEvent canvas width:"  << canvas->pv_width << "       height:" << canvas->pv_height << endl;
//    cout << "showEvent  scene width: " << scene->width()   << " scene height:" << scene->height()   << endl;
}

void MainWindow::on_actionRead_Input_File_triggered()
{
    // File --> Read Input File

    if (!Reader2::readDictionary("VDICT", base)) {
            cout << "Read dictionary VDICT failed." << endl;
        return;
    }
    // Check dictionary OK
    Dictionary::pVdict->writeDict();
    Instructions::writeInstructions();

    if (!Reader2::readDictionary("PRMDICT", base)) {
        cout << "Read dictionary PRMDICT failed." << endl;
        return;
    }
    // Check dictionary OK
    Dictionary::pPdict->writeDict();
    Instructions::writeInstructions();

    // Read standard parameters
    if (!Reader2::readStdPrm(base + "STANDARD.PRM")) {
        cout << "Read standard parameter file STANDARD.PRM failed." << endl;
        return;
    }

    QString desdir = QString::fromStdString(base) + "desdir\\";
    QString fileName = QFileDialog::getOpenFileName(this,
                       tr("Open Input File"), desdir,
                       tr("Input Files (*.inp)"));
    string filename = fileName.toStdString();
    if (Reader2::readInp(filename)) {
        canvas->zoomExtents();
        canvas->drawNetwork();
    }
}

void MainWindow::on_actionMentor_triggered()
{
    // Design --> Mentor
    MENTOR_Driver();
}

void MainWindow::on_actionMentMUX_triggered()
{
    // Design --> MentMUX
    Mentmux::mentorI("MENTOR-I");
    Canvas::my_canvas->drawNetwork();
}

void MainWindow::on_actionAll_triggered()
{
    // Display --> All
    ui->actionAll->setChecked(true);
    ui->actionBackbone->setChecked(false);
    ui->actionCluster->setChecked(false);
    ui->actionTree->setChecked(false);
    ui->actionDirect->setChecked(false);
    Canvas::show_network = "all";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionBackbone_triggered()
{
    // Display --> Backbone
    ui->actionAll->setChecked(false);
    ui->actionBackbone->setChecked(true);
    ui->actionCluster->setChecked(false);
    ui->actionTree->setChecked(false);
    ui->actionDirect->setChecked(false);
    Canvas::show_network = "backbone";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionCluster_triggered()
{
    // Display --> Cluster
    ui->actionAll->setChecked(false);
    ui->actionBackbone->setChecked(false);
    ui->actionCluster->setChecked(true);
    ui->actionTree->setChecked(false);
    ui->actionDirect->setChecked(false);
    Canvas::show_network = "clusters";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionTree_triggered()
{
    // Display --> Tree
    ui->actionAll->setChecked(false);
    ui->actionBackbone->setChecked(false);
    ui->actionCluster->setChecked(false);
    ui->actionTree->setChecked(true);
    ui->actionDirect->setChecked(false);
    Canvas::show_network = "tree";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionDirect_triggered()
{
    // Display --> Direct
    ui->actionAll->setChecked(false);
    ui->actionBackbone->setChecked(false);
    ui->actionCluster->setChecked(false);
    ui->actionTree->setChecked(false);
    ui->actionDirect->setChecked(true);
    Canvas::show_network = "direct";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionAll_2_triggered()
{
    // Display --> Labels --> All
    ui->actionAll_2->setChecked(true);
    ui->actionBackbone_2->setChecked(false);
    ui->actionNone->setChecked(false);
    Canvas::show_labels = "all";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionBackbone_2_triggered()
{
    // Display --> Labels --> Backbone
    ui->actionAll_2->setChecked(false);
    ui->actionBackbone_2->setChecked(true);
    ui->actionNone->setChecked(false);
    Canvas::show_labels = "backbone";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionNone_triggered()
{
    // Display --> Labels --> None
    ui->actionAll_2->setChecked(false);
    ui->actionBackbone_2->setChecked(false);
    ui->actionNone->setChecked(true);
    Canvas::show_labels = "none";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionNormal_triggered()
{
    // Display --> Normal
    ui->actionNormal->setChecked(true);
    ui->actionUtilisation->setChecked(false);
    Canvas::show_type = "normal";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionUtilisation_triggered()
{
    // Display --> Utilisation
    ui->actionUtilisation->setChecked(true);
    ui->actionNormal->setChecked(false);
    Canvas::show_type = "utilisation";
    Canvas::my_canvas->update();
}

void MainWindow::on_actionStandard_triggered()
{
    // Report --> Standard
    Utils::writeReport();
}

void MainWindow::on_actionCSV_triggered()
{
    // Report --> CSV
    Utils::writeCSVReport();
}
