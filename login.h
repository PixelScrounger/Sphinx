#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

#include <QDialog>
#include <QSql>
#include <QSqlError>
#include <QDebug>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QRegExp>
#include <QSettings>

#include "register.h"
#include "mainwindow.h"
#include "selectdatabase.h"
#include "mymsgbox.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
    QSqlDatabase db;
    static QString getUsername() { return Login::username; }

signals:
    void sendSettingsToSelectDatabase(bool,QString,QStringList,bool);

private slots:
    void enableButton();

    void checkLogin();

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void on_closeButton_clicked();

    void on_dbButton_clicked();

    void throwDBError(QSqlError error);

private:
    Ui::Login *ui;
    static QString username;
    void fillUsername();
    QSettings *settings;
    void loadSettings();

    QPoint mpos;
};

#endif // LOGIN_H
