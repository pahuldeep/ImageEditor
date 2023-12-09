#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>

#include <QMap>
#include <QAction>
#include <QLabel>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

#include "plugin_interface.h"

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    Editor(QWidget *parent = nullptr);
    ~Editor();

private:
    void initUI();

    void createActions();
    void setupShortcuts();
    void connections();
    void loadPlugins();
    void load(QString &path, QMenu*, QToolBar*);

    void showImage(QString);
    void changeImage(int);


private slots:
    void openImage();
    void saveAs();

    void zoomIn();
    void zoomOut();
    void prevImage();
    void nextImage();
    void rotateRight();
    void rotateLeft();

    void pluginPerform();

private:

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *basicMenu;
    QMenu *advanceMenu;


    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolBar *basicToolBar;
    QToolBar *advanceToolBar;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QAction *openAction;
    QAction *saveAsAction;
    QAction *exitAction;

    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *rotateLeftAction;
    QAction *rotateRightAction;

    QAction *prevAction;
    QAction *nextAction;


    QString currentImagePath;
    QGraphicsPixmapItem *currentImage;

    QMap<QString, pluginInterface*> editPlugins;

};
#endif // EDITOR_H
