#ifndef IMAGE_BASE_H
#define IMAGE_BASE_H

#include <QMainWindow>

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>

#include <QAction>
#include <QLabel>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>


class image_base : public QMainWindow
{
    Q_OBJECT

public:
    explicit image_base(QWidget *parent = nullptr);
    ~image_base();

private:
    void initUI();
    void createActions();

    void setupShortcuts();

private slots:
    void openImage();
    void saveAs();
    void showImage(QString);

    void zoomIn();
    void zoomOut();
    void prevImage();
    void nextImage();


private:
    QMenu *fileMenu;
    QMenu *viewMenu;

    QToolBar *fileToolBar;
    QToolBar *viewToolBar;

    QStatusBar *mainStatusBar;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;
    QGraphicsPixmapItem *currentImage;

    QLabel *mainStatusLabel;
    QString currentImagePath;

    QAction *openAction;
    QAction *saveAsAction;
    QAction *exitAction;

    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *prevAction;
    QAction *nextAction;

};
#endif // IMAGE_BASE_H
