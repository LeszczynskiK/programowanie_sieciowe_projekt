#include "serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent) : QWidget(parent) {
    setFixedSize(860, 860);//Set window size
    setWindowTitle("Server Window");

    //Server status label
    statusLabel = new QLabel("Server is not running", this);
    statusLabel->setGeometry(10, 10, 300, 200);

    //Message log field
    messageLog = new QTextEdit(this);
    messageLog->setReadOnly(true);
    messageLog->setGeometry(120, 120, 540, 540);

    //Start server button
    startButton = new QPushButton("Start Server", this);
    startButton->setGeometry(10, 760, 150, 50);
    connect(startButton, &QPushButton::clicked, this, &ServerWindow::onStartButtonClicked);

    tcpServer = new QTcpServer(this);//Initialize TCP server
}

//Start button click
void ServerWindow::onStartButtonClicked() {
    if (tcpServer->listen(QHostAddress::Any, 12345)) {
        statusLabel->setText("Server is listening...");
        connect(tcpServer, &QTcpServer::newConnection, this, &ServerWindow::newConnection);
    } else {
        statusLabel->setText("Server could not start!");
    }
}

//Handle new connections
void ServerWindow::newConnection() {
    QTcpSocket *socket = tcpServer->nextPendingConnection(); // Get the next client connection
    connect(socket, &QTcpSocket::readyRead, this, &ServerWindow::readMessage);
    messageLog->append("Client connected.");
}

//Read message from client
void ServerWindow::readMessage() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        QByteArray data = socket->readAll();//Read data from the socket
        messageLog->append("Received message: " + data);//Append message to the log
        socket->write("Message received");//Respond to client
        socket->flush();//Flush the socket
    }
}
