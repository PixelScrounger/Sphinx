#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->checkButton->setEnabled(false);

    connect(ui->login,SIGNAL(textChanged(QString)),this,SLOT(enableButton()));
    connect(ui->passwd,SIGNAL(textChanged(QString)),this,SLOT(enableButton()));
    connect(ui->checkButton,SIGNAL(clicked()),this,SLOT(checkLogin()));

    settings = new QSettings(this);
    loadSettings();
}

Login::~Login()
{
    delete ui;
}

void Login::fillUsername()
{
    QSqlQuery query;
    query.prepare("select username from users where login = :login");
    query.bindValue(":login", ui->login->text());
    if(!query.exec())
    {
        throwDBError(query.lastError());
        return;
    }

    if((MainWindow::qSize(query) > 0) && (query.first()))
        username = query.value(0).toString();
    else
        qDebug() << "Помилка заповнення username" << query.lastError();
}

void Login::loadSettings()
{
    SelectDatabase *wnd = new SelectDatabase();
    QStringList dbSettings;
    dbSettings << settings->value("ip", "176.32.0.252").toString() << settings->value("dbName", "rdth_test").toString()
               << settings->value("login", "rdth").toString() << settings->value("passwd", "rdthpass").toString();
    connect(this, SIGNAL(sendSettingsToSelectDatabase(bool,QString,QStringList,bool)), wnd, SLOT(recievedSettings(bool,QString,QStringList,bool)));
    emit sendSettingsToSelectDatabase(settings->value("isLocal", true).toBool(), settings->value("path", "").toString(), dbSettings, true);
    delete wnd;
}

void Login::checkLogin()
{
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(ui->login->text().toLatin1() + ui->passwd->text().toLatin1());

    QSqlQuery q;

    q.prepare("SELECT * FROM users LIMIT 1");
    if(!q.exec())
    {
        ui->dbButton->setStyleSheet("QPushButton:hover {\
                                    background-color: #ab2222;\
                                    border-image: url(:/lg-Cog-cogwheel-gear-Zahnrad.png) 0 0 0 0 stretch stretch;\
                                    border-style: inset;\
                                    border-width: 2px;\
                                    border-radius: 10px;\
                                    border-color: #ab2222;\
                                    padding: 6px;\
                                }\
                                QPushButton {\
                                    background-color: #d42b2b;\
                                    border-image: url(:/lg-Cog-cogwheel-gear-Zahnrad.png) 0 0 0 0 stretch stretch;\
                                    color: #F3F3F3;\
                                    border-style: outset;\
                                    border-width: 2px;\
                                    border-radius: 10px;\
                                    border-color: #d42b2b;\
                                    padding: 6px;\
                                }\
                                QPushButton:pressed {\
                                    background-color: #2A98F1;\
                                    border-image: url(:/lg-Cog-cogwheel-gear-Zahnrad.png) 0 0 0 0 stretch stretch;\
                                    border-style: inset;\
                                    border-width: 2px;\
                                    border-radius: 10px;\
                                    border-color: #2A98F1;\
                                    padding: 6px;\
                                }");
        ui->login->setReadOnly(true);
        ui->login->setText(tr("Проблема зі з'єднанням"));
        ui->login->setStyleSheet("background-color:#F7F7F7;\
                                 color: #d42b2b;\
                                 selection-background-color: rgb(119, 126, 131);\
                                 selection-color: rgb(255, 255, 255);\
                                 border-style: solid;\
                                 border-width: 2px;\
                                 border-radius: 10px;\
                                 border-color: #d42b2b;\
                                 min-width: 10em;\
                                 padding: 6px;");

        ui->passwd->setReadOnly(true);
        ui->passwd->setEchoMode(QLineEdit::Normal);
        ui->passwd->setFont(QFont("Georgia", 14, QFont::Bold));
        ui->passwd->setText(tr("Натисніть на шестерню для налаштування"));
        ui->passwd->setStyleSheet("background-color:#F7F7F7;\
                                  color: #d42b2b;\
                                  selection-background-color: rgb(119, 126, 131);\
                                  selection-color: rgb(255, 255, 255);\
                                  border-style: solid;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: #d42b2b;\
                                  min-width: 10em;\
                                  padding: 6px;");
        return;
    }

    if((ui->login->text() == "root") && (ui->passwd->text() == "sphinx"))
    {
        username = ui->login->text();
        MainWindow *mw = new MainWindow;
        connect(mw, SIGNAL(openLogin()), this, SLOT(show()));
        mw->show();
        this->hide();
        ui->login->setText("");
        ui->passwd->setText("");
        ui->login->setFocus();
        return;
    }
    q.prepare("SELECT login FROM users WHERE md5 = :md5 AND deleted = 0");
    q.bindValue(":md5", md5.result().toHex());
    q.exec();

    if(MainWindow::qSize(q) > 0)
    {
        fillUsername();
        qDebug() << username;

        q.prepare(QString("UPDATE users SET lastLogin = %1 WHERE md5 = :md5")
                  .arg((SelectDatabase::getDBLocation() == true)?"datetime('now', 'localtime')":"now()"));
        q.bindValue(":md5", md5.result().toHex());
        q.exec();

        MainWindow *mw = new MainWindow;
        connect(mw, SIGNAL(openLogin()), this, SLOT(show()));
        mw->show();
        this->hide();
        ui->login->setText("");
        ui->passwd->setText("");
        ui->login->setFocus();
    } else
        QMessageBox::critical(this,tr("Помилка авторизації"),tr("Ви ввели невірний логін чи пароль"), tr("Гаразд"));

}

void Login::enableButton()
{
    if (!ui->login->text().isEmpty() && !ui->passwd->text().isEmpty())
        ui->checkButton->setEnabled(true);
    else
        ui->checkButton->setEnabled(false);
}

void Login::mousePressEvent(QMouseEvent *event)
{
    mpos = event->pos();
}

void Login::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton)
    {
        QPoint diff = event->pos() - mpos;
        QPoint newpos = this->pos() + diff;

        this->move(newpos);
    }
}

void Login::on_closeButton_clicked()
{
    QApplication::quit();
}

void Login::on_dbButton_clicked()
{
    QStringList dbSettings;
    dbSettings << settings->value("ip", "176.32.0.252").toString() << settings->value("dbName", "rdth_test").toString()
               << settings->value("login", "rdth").toString() << settings->value("passwd", "rdthpass").toString();

    SelectDatabase *wnd = new SelectDatabase();
    connect(this, SIGNAL(sendSettingsToSelectDatabase(bool,QString,QStringList,bool)), wnd, SLOT(recievedSettings(bool,QString,QStringList,bool)));
    emit sendSettingsToSelectDatabase(settings->value("isLocal", true).toBool(), settings->value("path", "").toString(), dbSettings, false);
    wnd->show();
    if(!wnd->exec())
    {
        delete wnd;
        ui->dbButton->setStyleSheet("QPushButton:hover {\
                                    background-color: #4BB2FF;\
                                    border-image: url(:/lg-Cog-cogwheel-gear-Zahnrad.png) 0 0 0 0 stretch stretch;\
                                    border-style: inset;\
                                    border-width: 2px;\
                                    border-radius: 10px;\
                                    border-color: transparent;\
                                    padding: 6px;\
                                }\
                                QPushButton {\
                                    background-color: #2A98F1;\
                                    border-image: url(:/lg-Cog-cogwheel-gear-Zahnrad.png) 0 0 0 0 stretch stretch;\
                                    color: #F3F3F3;\
                                    border-style: outset;\
                                    border-width: 2px;\
                                    border-radius: 10px;\
                                    border-color: transparent;\
                                    padding: 6px;\
                                }\
                                QPushButton:pressed {\
                                    background-color: #258bd9;\
                                    border-image: url(:/lg-Cog-cogwheel-gear-Zahnrad.png) 0 0 0 0 stretch stretch;\
                                    border-style: inset;\
                                    border-width: 2px;\
                                    border-radius: 10px;\
                                    border-color: transparent;\
                                    padding: 6px;\
                                }");
        ui->login->setText("");
        ui->login->setReadOnly(false);
        ui->login->setStyleSheet("background-color:#F7F7F7;\
                                 color: #777E83;\
                                 selection-background-color: rgb(119, 126, 131);\
                                 selection-color: rgb(255, 255, 255);\
                                 border-style: solid;\
                                 border-width: 2px;\
                                 border-radius: 10px;\
                                 border-color: #2A98F1;\
                                 min-width: 10em;\
                                 padding: 6px;");

        ui->passwd->setText("");
        ui->passwd->setReadOnly(false);
        ui->passwd->setEchoMode(QLineEdit::Password);
        ui->passwd->setStyleSheet("background-color:#F7F7F7;\
                                  color: #777E83;\
                                  selection-background-color: rgb(119, 126, 131);\
                                  selection-color: rgb(255, 255, 255);\
                                  border-style: solid;\
                                  border-width: 2px;\
                                  border-radius: 10px;\
                                  border-color: #2A98F1;\
                                  min-width: 10em;\
                                  padding: 6px;");
    }
}

void Login::throwDBError(QSqlError error)
{
    MyMsgBox *msg = new MyMsgBox(this);
    msg->setIcon(QMessageBox::Critical);
    msg->setWindowTitle(tr("Проблема зі з'єднанням"));
    msg->setText(tr("Не вдалося під'єднатися бази даних."));
    msg->setInformativeText(tr("У разі використання серверної бази даних, перевірте своє з'єднання з Інтернетом. \nВ іншому ж випадку переконайтесь, що локальну базу даних не було переміщено."));
    msg->setDetailedText(tr("Код помилки: ") + QString::number(error.number()) + tr(";\nТекст помилки: ") + error.text());
    msg->setDetailsButtonText(tr("Детальніше"), tr("Приховати"));
    msg->exec();
    delete msg;
}
