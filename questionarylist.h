#ifndef QUESTIONARYLIST_H
#define QUESTIONARYLIST_H

#include <QDialog>
#include "mainwindow.h"
#include "questionaryinfo.h"

namespace Ui {
class QuestionaryList;
}

class QuestionaryList : public QDialog
{
    Q_OBJECT

public:
    explicit QuestionaryList(QWidget *parent = 0);
    ~QuestionaryList();
    void updateTable();

signals:
    void sendDataToThemes(bool);
    void sendDataToQuestionary(int, QString, QString, QString, bool);
    void sendDataToQuestionaryInfo(int, QString, QString, QString, QString, QString);
    void sendDataToPrint(int, QString, QString, QString);

private slots:

    void on_infoButton_clicked();

    void on_themeButton_clicked();

    void recieveDataFromThemes(int id_thme, QString name_subject, QString name_section, QString name_theme);

    void on_tableWidget_itemSelectionChanged();

    void on_closeButton_clicked();

    void on_deleteButton_clicked();

    void on_addButton_clicked();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void throwDBError(QSqlError error);

    void on_printButton_clicked();

private:
    Ui::QuestionaryList *ui;
    int id_theme;
};

#endif // QUESTIONARYLIST_H



