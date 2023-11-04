#include "image_base.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    image_base window;

    window.setWindowTitle("Image Basics");
    window.show();

    return application.exec();
}
