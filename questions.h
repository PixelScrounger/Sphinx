#ifndef QUESTIONS_H
#define QUESTIONS_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class Questions;
}

class Questions : public QDialog
{
    Q_OBJECT

public:
    explicit Questions(QWidget *parent = 0);
    ~Questions();
    void updateTable();


signals:
    void sendDataToThemes(bool flag);
    void sendDataToQuestion(bool flag, int id_question, QString text_question, int row);

private slots:
    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_editButton_clicked();

    void on_tableWidget_itemSelectionChanged();

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void recieveDataFromThemes(int id_thme, QString name_subject, QString name_section, QString name_theme, bool themeBtn);

    void recieveDataFromAddQuestion(QString text, int aCount, int correctCount, int row, int qID);

    void on_themeButton_clicked();

    void setColumns();

    void throwDBError(QSqlError error);

    void paintRow(int row, int id_question);

private:
    Ui::Questions *ui;

    int id_theme;
};

#endif // QUESTIONS_H
