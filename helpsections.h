#ifndef HELPSECTIONS_H
#define HELPSECTIONS_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class HelpSections;
}

class HelpSections : public QDialog
{
    Q_OBJECT

public:
    explicit HelpSections(QWidget *parent = 0);
    ~HelpSections();
    void updateList();

private slots:
    void on_subjectBox_currentIndexChanged(const QString &arg1);

    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_editButton_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::HelpSections *ui;
};

#endif // HELPSECTIONS_H





