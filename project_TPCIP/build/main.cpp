#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include "client.h"
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;//create server
    Client client;//create client

    client.connectToServer("127.0.0.1");
    client.sendMessage("Hello from client!");


    return a.exec();
}
