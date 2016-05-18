#ifndef QUESTIONARYINFO_H
#define QUESTIONARYINFO_H

#include <QDialog>
#include "mainwindow.h"
#include "questionarylist.h"

namespace Ui {
class QuestionaryInfo;
}

class QuestionaryInfo : public QDialog
{
    Q_OBJECT

public:
    explicit QuestionaryInfo(QWidget *parent = 0);
    ~QuestionaryInfo();

signals:
    void sendDataToAddQuestion(int,QString,bool);

private slots:
    void recieveDataFromQuestionary(QString name, QString theme, QString qCount, QVector<QString> questions, QVector<QString> answersCount, QVector<int> id);
    void recieveDataFromQuestionaryList(int id_questionary, QString name, QString theme, QString qCount, QString date, QString creator);

    void on_closeButton_clicked();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void on_infoButton_clicked();

    void on_tableWidget_itemSelectionChanged();

    void throwDBError(QSqlError error);

private:
    Ui::QuestionaryInfo *ui;

};

#endif // QUESTIONARYINFO_H
