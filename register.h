#ifndef REGISTER_H
#define REGISTER_H

#include <QDialog>
#include "login.h"

namespace Ui {
class Register;
}

class Register : public QDialog
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = 0);
    ~Register();

private slots:

    void on_regButton_clicked();

    void throwDBError(QSqlError error);

    void recieveData(int id, QString login, QString username, QString email, QString wndTitle, QString btnText);

private:
    Ui::Register *ui;
    bool checkInput();
    int idUser;
    QString uLogin;
    QString uEmail;
};

#endif // REGISTER_H
