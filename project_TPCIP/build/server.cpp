#include "server.h"

//Constructor for the Server class
Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);//Initialize the QTcpServer instance
    if (server->listen(QHostAddress::Any, 12345)) { // Hear on port number
        qDebug() << "Server is listening...";//Print if successfully listening

        //Connect the newConnection signal to the newConnection slot to handle client connections
        connect(server, &QTcpServer::newConnection, this, &Server::newConnection);
    } else {//If not connected
        qDebug() << "Server could not start!";
    }
}

//Slot to handle new incoming connections
void Server::newConnection()
{
    socket = server->nextPendingConnection();//New client connection
    connect(socket, &QTcpSocket::readyRead, this, &Server::readMessage);//messages from client
    qDebug() << "Client connected.";//Message when client connected
}

//Slot to read messages sent from the client
void Server::readMessage()
{
    QByteArray data = socket->readAll();//Read data from socket
    qDebug() << "Received message:" << data;//Print message from client
    sendMessage("Message received");//Respond to client
}

//Method to send a message to the connected client
void Server::sendMessage(const QString &message)
{
    if (socket) {//Ff socket ok
        socket->write(message.toUtf8());//Write the message to the socket and send it to the client
        socket->flush();//Flush socket
    }
}
