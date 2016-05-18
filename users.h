#ifndef USERS_H
#define USERS_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class Users;
}

class Users : public QDialog
{
    Q_OBJECT

public:
    explicit Users(QWidget *parent = 0);
    ~Users();
    void updateTable();

signals:

    void sendDataToRegister(int,QString,QString,QString,QString,QString);

private slots:

    void throwDBError(QSqlError error);

    void setColumns();

    void on_closeButton_clicked();

    void on_deleteButton_clicked();

    void on_addButton_clicked();

    void on_editButton_clicked();

    void on_tableWidget_itemSelectionChanged();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

private:

    Ui::Users *ui;
    bool isDeleted;
};

#endif // USERS_H
