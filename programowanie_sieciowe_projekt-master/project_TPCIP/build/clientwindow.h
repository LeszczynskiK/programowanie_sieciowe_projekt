#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QTextEdit>
#include <QBuffer>
#include <QFileDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QTimer>
#include "clickablelabel.h"


class ClientWindow : public QWidget {
    Q_OBJECT
public:
    ClientWindow(QWidget *parent = nullptr);

private slots:
    void connectToServer();//Method to connect to the server
    void sendMessage();
    void readMessage();
    void onConnected();
    void sendImageToServer();
    void clearChat();//Slot for clearing the chat
    void shareScreen();//Share screen of desktop
    void showFullScreenImage();//Show bigger image
    void showFullScreenShare();//Show bigger image of screenshot
    void disconnectFromServer();//Disconnect from server connection
    void handleSocketError();//Information about connection problems
    void toggleScreenshotSending();//On/off screenshot sending method

private:
    void paintEvent(QPaintEvent *event);//background setter
    QTcpSocket *socket;//Socket to connect to the server
    QLabel *statusLabel;//Label to show connection status
    QLineEdit *ipInput;//Input for IP address
    QLineEdit *messageInput;//Input for messages
    QPushButton *sendButton;//Button to send messages
    QPushButton *connectButton;//Button to connect to server
    QTextEdit *messageLog;//Input for text send from server
    QPushButton *sendImageButton;//Button to sent image from client to server
    QList<QTcpSocket*> connectedSockets;//All clients conected to server
    QPushButton *deleteButton;//Clear chat
    QPushButton *disconnectButton;//Disconnect from current server connection
    QPixmap background;//Background txt
    ClickableLabel *screenshotLabel;//Screenshot miniature
    ClickableLabel *imageLabel;//Image miniature
    QPushButton *sendScreenButton;//Desktop sharing
    QImage receivedScreenshot;//Ss in full quality
    QImage receivedImage;//Image in full quality
    QTimer *screenshotTimer;//Screen is being send after this time
    bool sendingScreenshots = false;//Menage if you want to turn on/off screenshot sending
};

#endif // CLIENTWINDOW_H
