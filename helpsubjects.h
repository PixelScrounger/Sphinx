#ifndef HELPSUBJECTS_H
#define HELPSUBJECTS_H

#include <QDialog>
#include "mainwindow.h"


namespace Ui {
class HelpSubjects;
}

class HelpSubjects : public QDialog
{
    Q_OBJECT

public:
    explicit HelpSubjects(QWidget *parent = 0);
    ~HelpSubjects();

private slots:

    void updateSubjectsList();
    void updateSectionsList();
    void updateThemesList();
    void updateLists();
    void recieveData(bool flag);

    void closeEvent(QCloseEvent *e);

    void on_subjectsList_itemSelectionChanged();

    void on_sectionsList_itemSelectionChanged();

    void on_themesList_itemSelectionChanged();

    void on_addButton_clicked();

    void on_editButton_clicked();

    void on_rbActionsSubjects_clicked();

    void on_rbActionsSections_clicked();

    void on_rbActionsThemes_clicked();

    void filterSlot();

    void on_rbFilterSubjects_clicked();

    void on_rbFilterSections_clicked();

    void on_rbFilterThemes_clicked();

    void on_clearFiltersButton_clicked();

    void on_closeButton_clicked();

    void on_deleteButton_clicked();

    void on_multiDeleteButton_clicked();

    void on_themesList_doubleClicked(const QModelIndex &index);

    void throwDBError(QSqlError error);

signals:
    void sendData(int, QString, QString, QString, bool);

private:
    Ui::HelpSubjects *ui;
    QList<QListWidgetItem *> mainSubjectsList;
    QList<QListWidgetItem *> mainSectionsList;
    QList<QListWidgetItem *> mainThemesList;

    QList<QListWidgetItem *> filterSubjectsList;
    QList<QListWidgetItem *> filterSectionsList;
    QList<QListWidgetItem *> filterThemesList;

    bool canUpdateSectionsList();
    bool canUpdateThemesList();
    QString inputData(QString title, QString label, QString defaultTextValue);

    bool closeBehavior;
    bool behavior;
    bool multipleDelete;

    QString filters[3];
};

#endif // HELPSUBJECTS_H


