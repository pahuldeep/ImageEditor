#include <QApplication>

#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

#include <QDebug>
#include <QPluginLoader>

#include "editor.h"

Editor::Editor(QWidget *parent): QMainWindow(parent), fileMenu(nullptr), viewMenu(nullptr), currentImage(nullptr)
{
    initUI();
    loadPlugins();
}

Editor::~Editor(){
}

void Editor::initUI()
{
    this->resize(1024, 768);

    // setup menubar
    fileMenu = menuBar() -> addMenu("&FILE");
    viewMenu = menuBar() -> addMenu("&VIEW");
    basicMenu = menuBar() -> addMenu("&BASIC");
    advanceMenu = menuBar() -> addMenu("&ADVANCE");

    // setup toolbar
    fileToolBar = addToolBar("file");
    viewToolBar = addToolBar("view");
    basicToolBar = addToolBar("basic");
    advanceToolBar = addToolBar("&advance");


    // main area for image display
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    setCentralWidget(imageView);


    // setup status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);

    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("Image Information will be here!");

    createActions();

}

void Editor::createActions()
{
    openAction = new QAction("&Open", this);
    exitAction = new QAction("E&xit", this);
    saveAsAction = new QAction("&Save as", this);

    zoomInAction = new QAction("Zoom in", this);
    zoomOutAction = new QAction("Zoom Out", this);
    rotateLeftAction = new QAction("Rotate left", this);
    rotateRightAction = new QAction("Rotate right", this);


    prevAction = new QAction("&Previous Image", this);
    nextAction = new QAction("&Next Image", this);


    // add actions to menubars
    fileMenu->addAction(openAction);
    fileMenu->addAction(exitAction);
    fileMenu->addAction(saveAsAction);

    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);
    viewMenu->addAction(rotateLeftAction);
    viewMenu->addAction(rotateRightAction);

    viewMenu->addAction(prevAction);
    viewMenu->addAction(nextAction);

    // add actions to toolbars
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(exitAction);

    // fileToolBar->addAction(rotateLeftAction);
    // fileToolBar->addAction(rotateRightAction);
    // viewToolBar->addAction(zoomInAction);
    // viewToolBar->addAction(zoomOutAction);

    viewToolBar->addAction(prevAction);
    viewToolBar->addAction(nextAction);


    setupShortcuts();
    connections();
}

void Editor::connections()
{
    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));

    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveAs()));

    connect(zoomInAction, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(rotateRightAction, SIGNAL(triggered(bool)), this, SLOT(rotateRight()));
    connect(rotateLeftAction, SIGNAL(triggered(bool)), this, SLOT(rotateLeft()));

    connect(prevAction, SIGNAL(triggered(bool)), this, SLOT(prevImage()));
    connect(nextAction, SIGNAL(triggered(bool)), this, SLOT(nextImage()));
}

void Editor::openImage()
{
    QFileDialog dialog(this);

    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFiles);

    dialog.setNameFilter(tr("Images files (*.png *.jpg *.tiff);;Text files (*.txt);;All files (*)"));

    QStringList filePaths;
    if(dialog.exec()){
        filePaths = dialog.selectedFiles();

        if(filePaths.isEmpty()) {
            return;
        }
        showImage(filePaths.at(0));

    }
}

void Editor::showImage(QString path){

    imageScene -> clear();
    imageView -> resetTransform();

    QPixmap image(path);

    currentImage = imageScene->addPixmap(image);

    imageScene->update();
    imageView->setSceneRect(image.rect());

    QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width()).arg(image.height()).arg(QFile(path).size());
    mainStatusLabel->setText(status);

    currentImagePath = path;
}

void Editor::saveAs()
{
    if(currentImage == nullptr){
        QMessageBox::information(this, "Information", "Save error: No image found!");
    }

    QFileDialog dialog(this);

    dialog.setWindowTitle("Save Image As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    dialog.setNameFilter(tr("Images files (*.png *.jpg *.tiff);;Text files (*.txt);;All files (*)"));

    QStringList filename;
    if(dialog.exec()){
        filename = dialog.selectedFiles();
        currentImage->pixmap().save(filename.at(0));
    }else{
        QMessageBox::information(this, "Information", "Error, Bad Format!");
    }

}

void Editor::changeImage(int delta)
{
    QFileInfo current(currentImagePath);

    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.jpg" << "*tiff" << "*.bmp";

    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);

    int idx = fileNames.indexOf(current.fileName());
    int newIdx = idx + delta;

    if (newIdx >= 0 && newIdx < fileNames.size()) {
        showImage(dir.absoluteFilePath(fileNames.at(newIdx)));
        prevAction->setEnabled(newIdx > 0);
        nextAction->setEnabled(newIdx < fileNames.size() - 1);
    }
}

void Editor::prevImage(){
    changeImage(-1);
}
void Editor::nextImage(){
    changeImage(1);
}
void Editor::zoomIn(){
    imageView->scale(1.2, 1.2);
}
void Editor::zoomOut(){
    imageView->scale(1/1.2, 1/1.2);
}
void Editor::rotateRight(){
    imageView->rotate(90);
}
void Editor::rotateLeft(){
    imageView->rotate(-90);
}


void Editor::load(QString &path, QMenu *nameMenu, QToolBar *nameToolbar)
{
    QDir plugin_directory(path);

    QStringList nameFilters = {"*.so", "*.dylib", "*.dll"};
    QFileInfoList plugins = plugin_directory.entryInfoList(nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);

    foreach(QFileInfo plugin, plugins) {

        QPluginLoader loader(plugin.absoluteFilePath(), this);
        pluginInterface *plugin_pointer = dynamic_cast<pluginInterface*>(loader.instance());

        if(plugin_pointer){
            QAction *action = new QAction(plugin_pointer->name());


            if(nameToolbar == nullptr){
                nameMenu->addAction(action);
            }
            else{
                nameMenu->addAction(action);
                nameToolbar->addAction(action);
            }

            editPlugins[plugin_pointer->name()] = plugin_pointer;
            connect(action, SIGNAL(triggered(bool)), this, SLOT(pluginPerform()));
        } else{
            qDebug() << "bad plugin: " << plugin.absoluteFilePath();
        }
    }
}

void Editor::loadPlugins(){

    QString basic_dll = "D:/QTFramework/PROJECTS/image_Editor/plugin/Basics";
    load(basic_dll, basicMenu, basicToolBar);

    QString advance_dll = "D:/QTFramework/PROJECTS/image_Editor/plugin/Advance";
    load(advance_dll, advanceMenu, advanceToolBar);

}

void Editor::pluginPerform()
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "No image to edit.");
        return;
    }
    QAction *active_action = qobject_cast<QAction*>(sender());
    pluginInterface *plugin_ptr = editPlugins[active_action->text()];

    if(!plugin_ptr) {
        QMessageBox::information(this, "Information", "No plugin is found.");
        return;
    }

    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage().convertToFormat(QImage::Format_RGB888);

    Mat source = Mat(image.height(), image.width(), CV_8UC3, image.bits(), image.bytesPerLine());

    plugin_ptr->edit(source, source);

    QImage image_edited;
    if(source.channels() == 3){
        image_edited = QImage(source.data, source.cols, source.rows, source.step, QImage::Format_RGB888);
    }
    else{
        image_edited = QImage(source.data, source.cols, source.rows, source.step, QImage::Format_Indexed8);
    }

    pixmap = QPixmap::fromImage(image_edited);

    currentImage = imageScene->addPixmap(pixmap);
    imageScene->update();
    imageView->setSceneRect(pixmap.rect());

    QString status = QString("(editted image), %1x%2").arg(pixmap.width()).arg(pixmap.height());
    mainStatusLabel->setText(status);
}

void Editor::setupShortcuts()
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
