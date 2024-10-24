#include <QCoreApplication>
#include "client.h"
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;//Create server
    Client client;//Create client

    client.connectToServer("127.0.0.1");// Connect to the server running on ip

    return a.exec();
}
