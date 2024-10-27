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

    //Image send button
    sendImageButton = new QPushButton("Send Image to Server", this);
    sendImageButton->setGeometry(10, 720, 200, 50);
    connect(sendImageButton, &QPushButton::clicked, this, &ClientWindow::sendImageToServer);
}

void ClientWindow::connectToServer() {
    QString ipAddress = ipInput->text();//Get the IP address from the input field
    if (!ipAddress.isEmpty()) {
        socket->connectToHost(ipAddress, 12353);//Attempt to connect to the server
    } else {
        statusLabel->setText("Please enter a valid IP address.");
    }
}

void ClientWindow::sendMessage() {
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        qDebug() << "Sending message:" << message; // Logowanie wiadomości
        socket->write(message.toUtf8());
        socket->flush();
        messageInput->clear(); // Wyczyść pole po wysłaniu
    }
}


void ClientWindow::onConnected() {
    statusLabel->setText("Connected to server");
    sendMessage();//Send greeting
}

void ClientWindow::readMessage() {
    QByteArray data = socket->readAll();
    QImage image;
    if (image.loadFromData(data)) { //If data is picture
        //Scale image to 160 x 160
        QImage scaledImage = image.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        //Display picture
        QLabel *imageLabel = new QLabel(this);
        imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
        imageLabel->setGeometry(410, 150, 160, 160); // Zmiana rozmiaru QLabel na 160x160
        imageLabel->show();
    } else {
        QString serverMessage = QString::fromUtf8(data);//if data is txt type
        messageLog->setTextColor(Qt::red);//message from server is red
        messageLog->append("Server: " + serverMessage);//Only message from server
    }
}

void ClientWindow::sendImageToServer() {
    // Dialog to choose image file
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (fileName.isEmpty()) {
        return; // Finish if not taken
    }

    QImage image(fileName); // Use chosen file
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPEG"); // Save as jpeg

    if (socket->state() == QAbstractSocket::ConnectedState) { // Check if the socket is connected
        socket->write(byteArray); // Send the image to the server
        socket->flush(); // Push the data
        messageLog->append("Image sent to server."); // Log that the image was sent
    }
}

