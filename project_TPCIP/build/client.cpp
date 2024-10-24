#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
}

void Client::connectToServer(const QString &ipAddress)
{
    socket->connectToHost(ipAddress, 1234);

    //connect
    connect(socket, &QTcpSocket::connected, [this]() {
        qDebug() << "Connected to server.";
    });

    //get messages from server
    connect(socket, &QTcpSocket::readyRead, this, &Client::readMessage);
}

void Client::sendMessage(const QString &message)
{
    socket->write(message.toUtf8());
    socket->flush();
}

void Client::readMessage()
{
    QByteArray data = socket->readAll();
    qDebug() << "Message from server:" << data;
}
