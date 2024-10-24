#include "client.h"

//Constructor for the Client class
Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);//Initialize the QTcpSocket instance
}

//Method to connect to the server using the specified IP address
void Client::connectToServer(const QString &ipAddress)
{
    //Connect to the specified IP address on port 1234
    socket->connectToHost(ipAddress, 12345); // ... / Port number

    //Connect 'connected' to the onConnected slot
    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);

    //Connect 'readyRead'  to the readMessage slot to handle incoming messages
    connect(socket, &QTcpSocket::readyRead, this, &Client::readMessage);
}

//Method to send a message to the server
void Client::sendMessage(const QString &message)
{
    socket->write(message.toUtf8());// Write the message
    socket->flush();//flush socket
}

//Slot to read messages from the server
void Client::readMessage()
{
    QByteArray data = socket->readAll();//Read data
    qDebug() << "Message from server:" << data;
}

//Slot to informc succesfull connection to the server
void Client::onConnected()
{
    qDebug() << "Connected to server.";
    sendMessage("Hello from client!"); // Send a message to the server
}
