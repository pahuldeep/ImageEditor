#include "editor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Editor window;

    window.setWindowTitle("Image Editor");
    window.show();

    return app.exec();
}
