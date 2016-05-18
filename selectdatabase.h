#ifndef SELECTDATABASE_H
#define SELECTDATABASE_H

#include <QDialog>
#include "login.h"

namespace Ui {
class SelectDatabase;
}

class SelectDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit SelectDatabase(QWidget *parent = 0);
    ~SelectDatabase();
    static bool getDBLocation() { return isLocalDB; }

private slots:
    void radioBtnChanged();

    void on_createDB_clicked();

    void on_chooseDB_clicked();

    void on_checkConnection_clicked();

    void recievedSettings(bool dbLocation, QString path, QStringList dbSettings, bool isFirstLaunch);

    void saveSettings();

private:
    Ui::SelectDatabase *ui;
    QSqlDatabase globalDB;
    static bool isLocalDB;
};

#endif // SELECTDATABASE_H
