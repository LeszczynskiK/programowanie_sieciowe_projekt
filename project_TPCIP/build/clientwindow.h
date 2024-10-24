#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QTextEdit>

class ClientWindow : public QWidget {
    Q_OBJECT
public:
    ClientWindow(QWidget *parent = nullptr);

private slots:
    void connectToServer();//Method to connect to the server
    void sendMessage();
    void readMessage();
    void onConnected();

private:
    QTcpSocket *socket;//Socket to connect to the server
    QLabel *statusLabel;//Label to show connection status
    QLineEdit *ipInput;//Input for IP address
    QLineEdit *messageInput;//Input for messages
    QPushButton *sendButton;//Button to send messages
    QPushButton *connectButton;//Button to connect to server
    QTextEdit *messageLog;//Input for text send from server
};
#endif // CLIENTWINDOW_H
