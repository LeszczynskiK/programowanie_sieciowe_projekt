#include "start_window.h"

start_window::start_window(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Authorisation window");

    const int x =860;//x window size
    const int y=860;//y window size

    setFixedSize(x, y);//Set window size
    //Background image
    background = QPixmap("/home/krzysiek89/Desktop/QT_aplikacje/Programowanie_sieciowe/programowanie_sieciowe_projekt-master/project_TPCIP/bck.jpg").scaled(x, y);


    QFont font;
    font.setPointSize(15);//Font size - beginning initialization

    QFont font_status;
    font_status.setPointSize(21);

    //Text inputs (login and password)
    loginInput = new QLineEdit(this);//login
    loginInput->setGeometry(60, 120, 500, 60);
    loginInput->setFont(font);
    loginInput->setPlaceholderText("Enter login");

    passwordInput = new QLineEdit(this);//password
    passwordInput->setGeometry(60, 200, 500, 60);
    passwordInput->setFont(font);
    passwordInput->setPlaceholderText("Enter password");

    //login status(logged or not)
    statusLogin = new QLabel("Not logged...", this);
    statusLogin->setFont(font_status);
    statusLogin->setStyleSheet("color: yellow;");
    statusLogin->setGeometry(30, 30, 650, 70);

    //Initialize buttons
    login_to_session_button = new QPushButton("Login to app...", this);//login to app
    login_to_session_button->setFont(font);
    login_to_session_button->setGeometry(70, 500, 300, 60);
    connect(login_to_session_button, &QPushButton::clicked, this, &start_window::handleLoginAttempt);

    turn_on_button = new QPushButton("Turn on app...", this);//turn on app
    turn_on_button->setFont(font);
    turn_on_button->setGeometry(70, 580, 300, 60);
    turn_on_button->setVisible(false);//if login and password are not ok, button is not visible
    connect(turn_on_button, &QPushButton::clicked, this, &start_window::openApp);//if clicked, open server-client view

    exit_button = new QPushButton("Exit app...", this);//leave from app
    exit_button->setFont(font);
    exit_button->setGeometry(70, 660, 300, 60);
    connect(exit_button, &QPushButton::clicked, this, &start_window::exitApp);

}

void start_window::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, background);//Background
    QWidget::paintEvent(event);
}

QString start_window::hashPassword(const QString &password) {

    //Hash the password using SHA-256
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

void start_window::handleLoginAttempt() {//check correctivity of typped data...
    userLogin = loginInput->text();
    userPassword = passwordInput->text();


    //predefined hashed password
    QString hashedCorrectPassword = hashPassword("123");


    if (userLogin == "admin" && hashPassword(userPassword) == hashedCorrectPassword) {//check validation of login and password
        isLoggedIn = true;//let login - bool variable
        statusLogin->setText("Login successful! Ready to proceed...");//new status of data
        turn_on_button->setVisible(true);
    } else {
        isLoggedIn = false;//deny login - frong data written
        statusLogin->setText("Incorrect login or password. Try again.");//new status of data
        turn_on_button->setVisible(false);
    }
}

void start_window::exitApp()
{
    this->close();//turn off app...
}

void start_window::openApp()
{
    if (isLoggedIn) {
        //create client and server
        serverWindow = new ServerWindow(nullptr);
        clientWindow = new ClientWindow(nullptr);
        this->close();//close this window
        serverWindow->show();//show client and server
        clientWindow->show();
    }
}
