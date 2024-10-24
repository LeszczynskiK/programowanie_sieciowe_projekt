#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QDebug>

class Server : public QObject
{
    Q_OBJECT

public:
    Server(QObject *parent = nullptr);
    void sendMessage(const QString &message);

private slots:
    void newConnection();
    void readMessage();

private:
    QTcpServer *server;
    QTcpSocket *socket;
};

#endif // SERVER_H
