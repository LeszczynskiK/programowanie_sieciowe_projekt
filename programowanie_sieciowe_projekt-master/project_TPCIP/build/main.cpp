#include <QApplication>
#include "clientwindow.h"
#include "serverwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

   //Server
    ServerWindow serverWindow;
    serverWindow.show();

    //Client
    ClientWindow clientWindow;
    clientWindow.show();

    return app.exec();
}
