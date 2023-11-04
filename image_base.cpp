#include "image_base.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

image_base::image_base(QWidget *parent)
    : QMainWindow(parent),
      fileMenu(nullptr),
      viewMenu(nullptr),
      currentImage(nullptr)
{
    initUI();
    setupShortcuts();
}

image_base::~image_base(){}

void image_base::initUI()
{
    this->resize(1024, 768);

    // setup menubar
    fileMenu = menuBar() -> addMenu("&FILE");
    viewMenu = menuBar() -> addMenu("&VIEW");

    // setup toolbar
    fileToolBar = addToolBar("file");
    viewToolBar = addToolBar("view");

    // main area for image display
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    setCentralWidget(imageView);

    // setup status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);

    mainStatusBar -> addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("Image Information will be here!");

    createActions();
}

void image_base::createActions()
{
    // create actions, add them to menus
    openAction = new QAction("&Open", this);
    openAction->setIcon(QIcon(QDir::currentPath().remove("build").append("extra") + "/open.png"));

    fileMenu->addAction(openAction);

    exitAction = new QAction("E&xit", this);
    exitAction->setIcon(QIcon(QDir::currentPath().remove("build").append("extra") + "/exit.png"));
    fileMenu->addAction(exitAction);

    saveAsAction = new QAction("&Save as", this);
    fileMenu->addAction(saveAsAction);


    zoomInAction = new QAction("Zoom in", this);
    viewMenu->addAction(zoomInAction);

    zoomOutAction = new QAction("Zoom Out", this);
    viewMenu->addAction(zoomOutAction);

    prevAction = new QAction("&Previous Image", this);
    viewMenu->addAction(prevAction);

    nextAction = new QAction("&Next Image", this);
    viewMenu->addAction(nextAction);


    // add actions to toolbars
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(exitAction);

    viewToolBar->addAction(zoomInAction);
    viewToolBar->addAction(zoomOutAction);
    viewToolBar->addAction(prevAction);
    viewToolBar->addAction(nextAction);



    // connect the signals and slots
    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));

    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveAs()));

    connect(zoomInAction, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(prevAction, SIGNAL(triggered(bool)), this, SLOT(prevImage()));
    connect(nextAction, SIGNAL(triggered(bool)), this, SLOT(nextImage()));

}

void image_base::openImage()
{

    QFileDialog dialog(this);

    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));

    QStringList filePaths;
    if(dialog.exec()){
        filePaths = dialog.selectedFiles();
        showImage(filePaths.at(0));
    }
}

void image_base::showImage(QString path){

    imageScene -> clear();
    imageView -> resetTransform();

    QPixmap image(path);

//    imageScene->addPixmap(image);
    currentImage = imageScene->addPixmap(image);

    imageScene->update();
    imageView->setSceneRect(image.rect());

    QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width()).arg(image.height()).arg(QFile(path).size());
    mainStatusLabel->setText(status);

    currentImagePath = path;
}

void image_base::saveAs()
{
    if(currentImage == nullptr){
        QMessageBox::information(this, "Information", "Save error: No image found!");
    }

    QFileDialog dialog(this);

    dialog.setWindowTitle("Save Image");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));

    QStringList filename;
    if(dialog.exec()){
        filename = dialog.selectedFiles();
        currentImage->pixmap().save(filename.at(0));
    }else{
        QMessageBox::information(this, "Information", "Error, Bad Format!");
    }

}

void image_base::prevImage()
{
    QFileInfo current(currentImagePath);

    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";

    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);

    int idx = fileNames.indexOf(current.fileName());
    if(idx > 0){
        showImage(dir.absoluteFilePath(fileNames.at(idx - 1)));

    }else{
//        QMessageBox::information(this, "Information", "Current Image is the first one.");
        prevAction->setEnabled(0);
        nextAction->setEnabled(1);

    }
}

void image_base::nextImage()
{
    QFileInfo current(currentImagePath);

    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";

    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);

    int idx = fileNames.indexOf(current.fileName());
    if(idx < fileNames.size() - 1){
        showImage(dir.absoluteFilePath(fileNames.at(idx + 1)));

    }else{
//        QMessageBox::information(this, "Information", "Current Image is the last one.");
        nextAction->setEnabled(0);
        prevAction->setEnabled(1);
    }

}

void image_base::zoomIn()
{
    imageView->scale(1.2, 1.2);
}

void image_base::zoomOut()
{
    imageView->scale(1/1.2, 1/1.2);
}


void image_base::setupShortcuts()
{
    QList<QKeySequence> shortcuts;

    shortcuts.clear();
    shortcuts << Qt::Key_Plus << Qt::Key_Equal;
    zoomInAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Minus << Qt::Key_Underscore;
    zoomOutAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Up << Qt::Key_Left;
    prevAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Down << Qt::Key_Right;
    nextAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::CTRL + Qt::Key_Q;
    exitAction->setShortcuts(shortcuts);

}



