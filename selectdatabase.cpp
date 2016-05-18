#include "selectdatabase.h"
#include "ui_selectdatabase.h"

SelectDatabase::SelectDatabase(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectDatabase)
{
    ui->setupUi(this);

    this->setWhatsThis(tr("Форма, що слугує для налаштування бази даних. Дається можливість вибору між серверною та локальною базою даних."));
    ui->rbServer->setWhatsThis(tr("Натисніть для вибору серверної бази даних."));
    ui->rbLocal->setWhatsThis(tr("Натисніть для вибору локальної бази даних."));
    ui->path->setWhatsThis(tr("Поле, яке містить шлях до локальної бази даних. Значення цього поля заповнюється автоматично при налаштуванні локальної бази даних."));
    ui->ip->setWhatsThis(tr("Поле, в якому задається адреса сервера."));
    ui->dbName->setWhatsThis(tr("Поле, в якому задається назва серверної бази даних."));
    ui->login->setWhatsThis(tr("Поле, в якому задається логін користувача на сервері."));
    ui->passwd->setWhatsThis(tr("Поле, в якому задається пароль користувача на сервері."));
    ui->chooseDB->setWhatsThis(tr("Натисніть для вибору локальної бази даних."));
    ui->createDB->setWhatsThis(tr("Натисніть для створення локальної бази даних."));
    ui->checkConnection->setWhatsThis(tr("Натисніть для перевірки з'єднання із базою даних. Якшо з'єднання вдале, то налаштування з'єднання збережуться в реєстрі для подальшого використання."));
    connect(ui->rbLocal, SIGNAL(clicked()), this, SLOT(radioBtnChanged()));
    connect(ui->rbServer, SIGNAL(clicked()), this, SLOT(radioBtnChanged()));
    ui->rbServer->setChecked(true);
    emit radioBtnChanged();

    setModal(true);
}

SelectDatabase::~SelectDatabase()
{
    delete ui;
}

void SelectDatabase::radioBtnChanged()
{
    if(ui->rbServer->isChecked())
    {
        ui->frame->setEnabled(true);
        ui->frame_2->setEnabled(false);
    }
    else
    {
        ui->frame->setEnabled(false);
        ui->frame_2->setEnabled(true);
    }
}

void SelectDatabase::on_createDB_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Вкажіть шлях, де зберігатиметься база даних"), QApplication::applicationDirPath(), "SQLite-файли (*.sqlite)");
    if(path.isEmpty())
        return;

    QFile mFile(QDir::toNativeSeparators(path));
    if(!mFile.open(QIODevice::WriteOnly))
        return;
    mFile.close();

    ui->path->setText(path);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    if(!db.open())
    {
        ui->lblStatus->setStyleSheet("color:red;");
        ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
        return;
    }

    QSqlQuery q;
    q.prepare("CREATE TABLE users (id integer NOT NULL PRIMARY KEY AUTOINCREMENT,login varchar NOT NULL,md5 varchar,username varchar,email varchar,createat timestamp,lastLogin timestamp DEFAULT CURRENT_TIMESTAMP,deleted integer NOT NULL DEFAULT 0)");
    if(!q.exec())
    {
        ui->lblStatus->setStyleSheet("color:red;");
        ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
        return;
    }

    q.prepare("CREATE TABLE subjects (id_subject integer NOT NULL PRIMARY KEY AUTOINCREMENT,name_subject varchar NOT NULL,deleted integer NOT NULL DEFAULT 0)");
    if(!q.exec())
    {
        ui->lblStatus->setStyleSheet("color:red;");
        ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
        return;
    }

    q.prepare("CREATE TABLE sections (id_section integer NOT NULL PRIMARY KEY AUTOINCREMENT,id_subject integer NOT NULL,name_section varchar NOT NULL,deleted integer NOT NULL DEFAULT 0)");
    if(!q.exec())
    {
        ui->lblStatus->setStyleSheet("color:red;");
        ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
        return;
    }

    q.prepare("CREATE TABLE themes (id_theme integer NOT NULL PRIMARY KEY AUTOINCREMENT,id_section integer NOT NULL,name_theme varchar NOT NULL,deleted integer NOT NULL DEFAULT 0)");
    if(!q.exec())
    {
        ui->lblStatus->setStyleSheet("color:red;");
        ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
        return;
    }

    q.prepare("CREATE TABLE questions (id_question integer NOT NULL PRIMARY KEY AUTOINCREMENT,id_theme integer NOT NULL,text_question varchar NOT NULL,createdBy varchar NOT NULL,date_created timestamp NOT NULL,date_edit timestamp NOT NULL,deleted integer NOT NULL DEFAULT 0)");
    if(!q.exec())
    {
        ui->lblStatus->setStyleSheet("color:red;");
        ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
        return;
    }

    q.prepare("CREATE TABLE answers (id_answer integer NOT NULL PRIMARY KEY AUTOINCREMENT,id_question integer NOT NULL,text_answer varchar NOT NULL,correct integer NOT NULL DEFAULT 0,deleted integer NOT NULL DEFAULT 0)");
    if(!q.exec())
    {
        ui->lblStatus->setStyleSheet("color:red;");
        ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
        return;
    }

    q.prepare("CREATE TABLE questionary (id_questionary integer NOT NULL PRIMARY KEY AUTOINCREMENT,name_questionary varchar NOT NULL,createdBy varchar NOT NULL,date_created timestamp NOT NULL,id_theme integer NOT NULL)");
    if(!q.exec())
    {
        ui->lblStatus->setStyleSheet("color:red;");
        ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
        return;
    }

    q.prepare("CREATE TABLE questions_questionary (id_question_q integer NOT NULL PRIMARY KEY AUTOINCREMENT,id_questionary integer NOT NULL,id_question integer NOT NULL)");
    if(!q.exec())
    {
        ui->lblStatus->setStyleSheet("color:red;");
        ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
        return;
    }

    db.close();
    ui->lblStatus->setStyleSheet("color:green;");
    ui->lblStatus->setText("Базу даних успішно створено");
}

void SelectDatabase::on_chooseDB_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Вкажіть шлях до бази даних"), QApplication::applicationDirPath(), "SQLite-файли (*.sqlite)");
    if(path.isEmpty())
        return;

    ui->path->setText(path);
    ui->lblStatus->setStyleSheet("color:green;");
    ui->lblStatus->setText(tr("Базу даних успішно обрано"));
}

void SelectDatabase::on_checkConnection_clicked()
{
    if(ui->rbServer->isChecked())
    {
        globalDB = QSqlDatabase::addDatabase("QMYSQL");
        globalDB.setHostName(ui->ip->text());
        globalDB.setDatabaseName(ui->dbName->text());
        globalDB.setUserName(ui->login->text());
        globalDB.setPassword(ui->passwd->text());

        if(!globalDB.open())
        {
            ui->lblStatus->setStyleSheet("color:red;");
            ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
            return;
        }

        QSqlQuery q;
        if(!q.exec("SELECT * FROM users LIMIT 1"))
        {
            ui->lblStatus->setStyleSheet("color:red;");
            ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
            return;
        }

        ui->lblStatus->setStyleSheet("color:green;");
        ui->lblStatus->setText(tr("Успішне з'єднання із базою даних. Ваші налаштування були збережені"));

        int dlgResult;
        QMessageBox msg;
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle(tr("Успішне з'єднання"));
        msg.setText(tr("З'єднання із базою даних успішне. Ваші налаштування були збережені. Бажаєте закрити це вікно та продовжити сеанс?"));
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, tr("Так"));
        msg.setButtonText(QMessageBox::No, tr("Ні"));
        dlgResult = msg.exec();

        if(dlgResult == QMessageBox::No)
        {
            globalDB.close();
            ui->lblStatus->setStyleSheet("");
            ui->lblStatus->setText(tr("Оберіть базу даних та перевірте з'єднання"));
            return;
        }
        else
        {
            isLocalDB = false;
            saveSettings();
            this->close();
        }
    }
    else
    {
        globalDB = QSqlDatabase::addDatabase("QSQLITE");
        globalDB.setDatabaseName(ui->path->text());
        if(!globalDB.open())
        {
            ui->lblStatus->setStyleSheet("color:red;");
            ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
            return;
        }

        QSqlQuery q;
        if(!q.exec("SELECT * FROM users LIMIT 1"))
        {
            ui->lblStatus->setStyleSheet("color:red;");
            ui->lblStatus->setText(tr("Не вдалося з'єднатися із базою даних"));
            return;
        }

        ui->lblStatus->setStyleSheet("color:green;");
        ui->lblStatus->setText(tr("Успішне з'єднання із базою даних. Ваші налаштування були збережені"));

        int dlgResult;
        QMessageBox msg;
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle(tr("Успішне з'єднання"));
        msg.setText(tr("З'єднання із базою даних успішне. Ваші налаштування були збережені. Бажаєте закрити це вікно та продовжити сеанс?"));
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, tr("Так"));
        msg.setButtonText(QMessageBox::No, tr("Ні"));
        dlgResult = msg.exec();

        if(dlgResult == QMessageBox::No)
        {
            globalDB.close();
            ui->lblStatus->setStyleSheet("");
            ui->lblStatus->setText(tr("Оберіть базу даних та перевірте з'єднання"));
            return;
        }
        else
        {
            isLocalDB = true;
            saveSettings();
            this->close();
        }
    }
}

void SelectDatabase::recievedSettings(bool dbLocation, QString path, QStringList dbSettings, bool isFirstLaunch)
{
    isLocalDB = dbLocation;

    if(isFirstLaunch)
    {
        if(isLocalDB)
        {
            qDebug() << "Recieved something from local";
            if(path.isEmpty())
                return;

            globalDB = QSqlDatabase::addDatabase("QSQLITE");
            globalDB.setDatabaseName(path);
            globalDB.open();
        }
        else
        {
            qDebug() << "Recieved something from server";
            if(dbSettings[0].isEmpty())
                return;

            globalDB = QSqlDatabase::addDatabase("QMYSQL");
            globalDB.setHostName(dbSettings[0]);
            globalDB.setDatabaseName(dbSettings[1]);
            globalDB.setUserName(dbSettings[2]);
            globalDB.setPassword(dbSettings[3]);
            globalDB.open();
        }
    }
    else
    {
        ui->path->setText(path);
        ui->ip->setText(dbSettings[0]);
        ui->dbName->setText(dbSettings[1]);
        ui->login->setText(dbSettings[2]);
        ui->passwd->setText(dbSettings[3]);

        if(isLocalDB)
            ui->rbLocal->setChecked(true);
        else
            ui->rbServer->setChecked(true);

        emit radioBtnChanged();
    }
}

void SelectDatabase::saveSettings()
{
    QSettings settings;
    if(ui->rbServer->isChecked())
    {
        settings.setValue("isLocal", false);
        settings.setValue("ip", ui->ip->text());
        settings.setValue("dbName", ui->dbName->text());
        settings.setValue("login", ui->login->text());
        settings.setValue("passwd", ui->passwd->text());
    }
    else
    {
        settings.setValue("isLocal", true);
        settings.setValue("path", ui->path->text());
    }
}
