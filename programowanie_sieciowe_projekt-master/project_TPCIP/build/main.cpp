#include <QApplication>
#include "start_window.h"
#include "clientwindow.h"
#include "serverwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    //display authorisation window
    start_window startWindow;
    startWindow.show();

    return app.exec();
}
