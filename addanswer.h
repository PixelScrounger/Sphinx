#ifndef ADDANSWER_H
#define ADDANSWER_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class AddAnswer;
}

class AddAnswer : public QDialog
{
    Q_OBJECT

public:
    explicit AddAnswer(QWidget *parent = 0);
    ~AddAnswer();

private:
    Ui::AddAnswer *ui;
    bool _flag;
    bool correct;
    int idA;
    int idQ;

private slots:
    void enableButton();
    void on_cancelButton_clicked();
    void on_addButton_clicked();
    void throwDBError(QSqlError error);

    void recievedData(QString text, QString buttonLabel, QString windowLabel, int idAnswer, int idQuestion, bool correct, bool flag);
};

#endif // ADDANSWER_H



