#include "clientwindow.h"



ClientWindow::ClientWindow(QWidget *parent) : QWidget(parent) {

    setWindowTitle("Client Window");

    const int x =860;
    const int y=860;

    setFixedSize(x, y);//Window size
    background = QPixmap("/home/krzysiek89/Desktop/QT_aplikacje/Programowanie_sieciowe/programowanie_sieciowe_projekt-master/project_TPCIP/bck.jpg").scaled(x, y);


    QFont font;
    font.setPointSize(15);//Font size - beginning initialization

    //Initialize IP address input
    ipInput = new QLineEdit(this);
    ipInput->setGeometry(10, 10, 200, 50);
    ipInput->setFont(font);
    ipInput->setPlaceholderText("Enter server IP address");

    //Initialize status label
    statusLabel = new QLabel("Disconnected", this);
    statusLabel->setFont(font);
    statusLabel->setStyleSheet("color: yellow;");
    statusLabel->setGeometry(220, 10, 200, 50);

    //Initialize message input
    messageInput = new QLineEdit(this);
    messageInput->setFont(font);
    messageInput->setGeometry(10, 90, 400, 50);

    //Initialize buttons
    sendButton = new QPushButton("Send", this);
    sendButton->setFont(font);
    sendButton->setGeometry(420, 20, 120, 40);

    connectButton = new QPushButton("Connect...", this);
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

    //Desktop sharing
    QPushButton *sendScreenshotButton = new QPushButton("Send Screenshot", this);
    sendScreenshotButton->setGeometry(410, 720, 200, 50);
    connect(sendScreenshotButton, &QPushButton::clicked, this, &ClientWindow::shareScreen);


    //Initialize delete button
    QPushButton *deleteButton = new QPushButton("Clear Chat", this);
    deleteButton->setFont(font);
    deleteButton->setGeometry(250, 720, 120, 50); // Geometry for the delete button

    //Connect the delete button signal to the corresponding slot
    connect(deleteButton, &QPushButton::clicked, this, &ClientWindow::clearChat);

    //Add frame based on QLabel
    QLabel *frame = new QLabel(this);
    frame->setGeometry(600, 210, 180, 180);//Pos and size
    frame->setStyleSheet("border: 2px solid grey;");//Style of frame

    QLabel *frame1 = new QLabel(this);
    frame1->setGeometry(600, 450, 180, 180);//Pos and size
    frame1->setStyleSheet("border: 2px solid grey;");//Style of frame
}

void ClientWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, background);//Background
    QWidget::paintEvent(event);
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
        qDebug() << "Sending message:" << message;//Log messages
        socket->write(message.toUtf8());
        socket->flush();
        messageInput->clear();//Clear field after sent
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
        if (image.loadFromData(data)) { //If data is an image
            //Find all QLabels except added frames
            QList<QLabel *> labels = findChildren<QLabel *>();
            for (QLabel *label : labels) {
                //Is frame?
                if (label->geometry() != QRect(600, 210, 180, 180) &&
                    label->geometry() != QRect(600, 450, 180, 180)) {
                    delete label;//Delete all exept for frame
                }
            }
        }

        //Scale image to 160 x 160
        QImage scaledImage = image.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        //Display picture
        QLabel *imageLabel = new QLabel(this);
        imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
        imageLabel->setGeometry(608, 218, 160, 160);//Scale and set pos
        imageLabel->show();
    } else {
        QString serverMessage = QString::fromUtf8(data);//if data is txt type
        messageLog->setTextColor(Qt::red);//message from server is red
        messageLog->append("Server: " + serverMessage);//Only message from server
    }
}

void ClientWindow::sendImageToServer() {
    //Dialog to choose image file
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

void ClientWindow::clearChat() {
   messageLog->clear();
}

void ClientWindow::shareScreen() {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    QPixmap screenshot = screen->grabWindow(0);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    if (buffer.open(QIODevice::WriteOnly)) {
        if (screenshot.save(&buffer, "JPEG")) {
            if (socket->state() == QAbstractSocket::ConnectedState) {
                // add prefix screenshod to make possible recoglise ss from classig image
                QByteArray dataToSend = "SCREENSHOT" + byteArray;
                socket->write(dataToSend);
                socket->flush();
            } else {
                messageLog->append("Socket not connected.");
            }
        } else {
            messageLog->append("Failed to save screenshot.");
        }
    } else {
        messageLog->append("Failed to open buffer.");
    }
}

