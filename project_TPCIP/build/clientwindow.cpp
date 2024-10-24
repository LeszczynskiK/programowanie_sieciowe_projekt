#include "clientwindow.h"


ClientWindow::ClientWindow(QWidget *parent) : QWidget(parent) {
    setFixedSize(860, 860);//Window size
    setWindowTitle("Client Window");

    QFont font;
    font.setPointSize(12);//Font size - beginning initialization

    //Initialize IP address input
    ipInput = new QLineEdit(this);
    ipInput->setGeometry(10, 10, 200, 50);
    ipInput->setFont(font);
    ipInput->setPlaceholderText("Enter server IP address");

    //Initialize status label
    statusLabel = new QLabel("Disconnected", this);
    statusLabel->setFont(font);
    statusLabel->setGeometry(220, 10, 200, 50);

    //Initialize message input
    messageInput = new QLineEdit(this);
    messageInput->setFont(font);
    messageInput->setGeometry(10, 90, 400, 50);

    //Initialize buttons
    sendButton = new QPushButton("Send Message", this);
    sendButton->setFont(font);
    sendButton->setGeometry(420, 20, 120, 40);

    connectButton = new QPushButton("Connect to Server", this);
    connectButton->setFont(font);
    connectButton->setGeometry(550, 20, 150, 40); //Add geometry for the connect button

    //Connect signals and slots
    connect(connectButton, &QPushButton::clicked, this, &ClientWindow::connectToServer);
    connect(sendButton, &QPushButton::clicked, this, &ClientWindow::sendMessage);

    //Initialize socket
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &ClientWindow::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &ClientWindow::readMessage);

    //Messages from server
    messageLog = new QTextEdit(this);
    messageLog->setReadOnly(true);
    messageLog->setGeometry(10, 150, 540, 540);
    messageLog->setFont(font);
}

void ClientWindow::connectToServer() {
    QString ipAddress = ipInput->text();//Get the IP address from the input field
    if (!ipAddress.isEmpty()) {
        socket->connectToHost(ipAddress, 12349);//Attempt to connect to the server
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

void ClientWindow::onConnected() {
    statusLabel->setText("Connected to server");
    sendMessage();//Send greeting
}

void ClientWindow::readMessage() {
    QByteArray data = socket->readAll();
    QString serverMessage = QString::fromUtf8(data);

    messageLog->setTextColor(Qt::red);//Message from server is red
    messageLog->append("Server: " + serverMessage);//Only message from server
}
