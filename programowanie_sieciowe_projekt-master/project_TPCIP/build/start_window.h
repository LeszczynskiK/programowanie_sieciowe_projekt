#ifndef START_WINDOW_H
#define START_WINDOW_H

#include "serverwindow.h"
#include "clientwindow.h"

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QTextEdit>
#include <QPainter>
#include <QPaintEvent>

class start_window : public QWidget {
    Q_OBJECT
public:
    start_window(QWidget *parent = nullptr);

private slots:
    void paintEvent(QPaintEvent *event);//background setter
    void exitApp();
    void openApp();
    void handleLoginAttempt();

private:
    QPixmap background;//Background txt
    QLineEdit *loginInput;//type login
    QLineEdit *passwordInput;//type password
    QPushButton *login_to_session_button;//login to app
    QPushButton *turn_on_button;//open app
    QPushButton *exit_button;//exit app
    QLabel *statusLogin;//Label to show status of login to app
    ServerWindow *serverWindow;//server object
    ClientWindow *clientWindow;//client object
    QString userLogin;//Store typped login
    QString userPassword;//Store typped password
    bool isLoggedIn = false;//If password and login are typped correctly
};

#endif // START_WINDOW_H
