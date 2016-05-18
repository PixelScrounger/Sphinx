#ifndef QUESTIONARY_H
#define QUESTIONARY_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class Questionary;
}

class Questionary : public QDialog
{
    Q_OBJECT

public:
    explicit Questionary(QWidget *parent = 0);
    ~Questionary();
    void updateTable();

signals:
    void sendDataToThemes(bool flag);
    void sendDataToQuestionEdit(bool flag, int id_question, QString text_question, int row);
    void sendDataToQuestions(int, QString, QString, QString, bool);
    void sendDataToQuestionaryInfo(QString, QString, QString, QVector<QString>, QVector<QString>, QVector<int>);
    void sendDataToPrint(int, QString, QString, QString);

private slots:

    void on_addButton_clicked();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void recieveDataFromQuestions(QString qText, int aCount, int correctAnswersCount, int row);

    void recieveDataFromThemes(int id_thme, QString name_subject, QString name_section, QString name_theme);

    void recieveDataFromQuestionaryList(int id_thme, QString name_subject, QString name_section, QString name_theme, bool themeBtn);

    void on_themeButton_clicked();

    void on_editButton_clicked();

    void on_randomButton_clicked();

    void checkLabelGrammar();

    bool askForChoice(int problemQuestionNumber, QString message);

    void fillNameLine(QString themeName);

    void throwDBError(QSqlError error);

private:
    Ui::Questionary *ui;
    int id_theme;
};

#endif // QUESTIONARY_H


