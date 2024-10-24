#include "clientwindow.h"


ClientWindow::ClientWindow(QWidget *parent) : QWidget(parent) {
    setFixedSize(860, 860);//Window size
    setWindowTitle("Client Window");

    //Initialize IP address input
    ipInput = new QLineEdit(this);
    ipInput->setGeometry(10, 10, 200, 30);
    ipInput->setPlaceholderText("Enter server IP address");

    //Initialize status label
    statusLabel = new QLabel("Disconnected", this);
    statusLabel->setGeometry(220, 10, 200, 30);

    //Initialize message input
    messageInput = new QLineEdit(this);
    messageInput->setGeometry(10, 50, 400, 30);

    //Initialize buttons
    sendButton = new QPushButton("Send Message", this);
    sendButton->setGeometry(420, 50, 100, 30);

    connectButton = new QPushButton("Connect to Server", this);
    connectButton->setGeometry(530, 50, 130, 30); //Add geometry for the connect button

    //Connect signals and slots
    connect(connectButton, &QPushButton::clicked, this, &ClientWindow::connectToServer);
    connect(sendButton, &QPushButton::clicked, this, &ClientWindow::sendMessage);

    //Initialize socket
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &ClientWindow::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &ClientWindow::readMessage);
}

void ClientWindow::connectToServer() {
    QString ipAddress = ipInput->text();//Get the IP address from the input field
    if (!ipAddress.isEmpty()) {
        socket->connectToHost(ipAddress, 12345);//Attempt to connect to the server
    } else {
        statusLabel->setText("Please enter a valid IP address.");
    }
}

void ClientWindow::sendMessage() {
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        socket->write(message.toUtf8());
        socket->flush();
        messageInput->clear();//Clear the input after sending
    }
}

void ClientWindow::readMessage() {
    QByteArray data = socket->readAll();
    qDebug() << "Message from server:" << data;
}

void ClientWindow::onConnected() {
    statusLabel->setText("Connected to server");
    sendMessage();//Send greeting
}
