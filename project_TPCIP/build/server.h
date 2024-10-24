#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QDebug>

class Server : public QObject{
    Q_OBJECT
public:
    Server(QObject *parent = nullptr);
    void sendMessage(const QString &message);
    QTcpServer *server;
    QTcpSocket *socket;
public slots:
    void newConnection();
    void readMessage();
};

#endif // SERVER_H
