#include <QApplication>
#include "start_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    //display authorisation window
    start_window startWindow;
    startWindow.show();

    return app.exec();
}
