#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);

    //hear on port nr...
    if (server->listen(QHostAddress::Any, 1234)) {
        qDebug() << "Server is listening...";
        connect(server, &QTcpServer::newConnection, this, &Server::newConnection);
    } else {
        qDebug() << "Server could not start!";
    }
}

void Server::newConnection()
{
    socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &Server::readMessage);
    qDebug() << "Client connected.";
}

void Server::readMessage()
{
    QByteArray data = socket->readAll();
    qDebug() << "Received message:" << data;

   //respond to client
    sendMessage("Message received");
}

void Server::sendMessage(const QString &message)
{
    if (socket) {
        socket->write(message.toUtf8());
        socket->flush();
    }
}
