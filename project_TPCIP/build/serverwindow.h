
#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QLineEdit>
#include <QBuffer>
#include <QFileDialog>

class ServerWindow : public QWidget {
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);

private slots:
    void onStartButtonClicked();//Start button click
    void newConnection();//Handling new connections
    void readMessage();//Read messages from the client
    void sendMessageToClient();//Send message to client
    void clientDisconnected();//Clear after disconected client
    void sendImageToClient();//Send image to client
    void clearChat();//Clear chat

private:
    QTcpSocket *socket;//Socket to connect to the server
    QTcpServer *tcpServer;//Pointer to the TCP server instance
    QLabel *statusLabel;//Label to display server status
    QTextEdit *messageLog;//Text area to display received messages
    QPushButton *startButton;//Button to start the server
    QLineEdit *messageInput;//Input for messages to client
    QPushButton *sendButton;//Button to send message to client
    QList<QTcpSocket*> connectedSockets;//All clients conected to server
    QPushButton *sendImageButton;//Button to send image from server to client
    QPushButton *deleteButton;//Clear chat button
};
#endif // SERVERWINDOW_H
