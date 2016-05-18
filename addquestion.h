#ifndef ADDQUESTION_H
#define ADDQUESTION_H

#include <QDialog>
#include "mainwindow.h"
#include "addanswer.h"

namespace Ui {
class AddQuestion;
}

class AddQuestion : public QDialog
{
    Q_OBJECT

public:
    explicit AddQuestion(QWidget *parent = 0);
    ~AddQuestion();
    void updateTable();

    virtual void closeEvent(QCloseEvent *e);

signals:

    void sendDataToAddAnswer(QString text, QString buttonLabel, QString windowLabel, int idAnswer, int idQuestion, bool correct, bool flag);

    void sendDataToQuestionary(QString qText, int aCount, int correctAnswersCount, int row);

    void sendDataToQuestions(QString qText, int aCount, int correctAnswersCount, int row, int qID);

private slots:
    bool changeData();

    void on_addAnswerButton_clicked();

    void on_deleteButton_clicked();

    void on_editButton_clicked();

    void on_tableWidget_itemSelectionChanged();

    void recievedData(bool flag, int id_q, QString text_question, int row);

    void recievedDataFromQuestionaryList(int id_q, QString text_question, bool flag);

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void setColumns();

    void throwDBError(QSqlError error);

private:
    Ui::AddQuestion *ui;

    bool checkForCorrectAnswers();
    bool _flag;
    int id_question;
    bool isOpenedFromQuestionaryList;
    int tblRow;
};

#endif // ADDQUESTION_H



