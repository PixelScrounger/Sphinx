#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);

    this->setWhatsThis(tr("Форма, що слугує для реєстрації новоих користувачів, а також редагування існуючих."));
    ui->crLogin->setWhatsThis(tr("Поле для введення логіну користувача."));
    ui->crEmail->setWhatsThis(tr("Поле для введення електронної пошти користувача."));
    ui->crName->setWhatsThis(tr("Поле для введення прізвища, ім'я та по-батькові користувача."));
    ui->crPasswd->setWhatsThis(tr("Поле для введення пароля користувача."));
    ui->crPasswd2->setWhatsThis(tr("Поле для підтвердження пароля користувача. Значення цього поля повинне співпадати зі значенням попереднього."));
    ui->regButton->setWhatsThis(tr("Натисніть для реєстрації/редагування користувача."));
    ui->cancelButton->setWhatsThis(tr("Натисніть для відміни дій реєстрації/редагування."));


    QRegExp expLogin("^[a-zA-Z0-9]{2,20}\\S+$");
    ui->crLogin->setValidator(new QRegExpValidator(expLogin,this));
    ui->crPasswd->setValidator(new QRegExpValidator(expLogin,this));

    QRegExp expName("[a-zA-Zа-яА-ЯіІїЇєЄ]{1,15}\\s[a-zA-Zа-яА-ЯіІїЇєЄ]{1,15}\\s[a-zA-Zа-яА-ЯіІїЇєЄ]{1,15}");
    ui->crName->setValidator(new QRegExpValidator(expName,this));

    idUser = 0;
}

Register::~Register()
{
    delete ui;
}

void Register::on_regButton_clicked()
{
    if (ui->crEmail->text().isEmpty() || ui->crLogin->text().isEmpty() || ui->crName->text().isEmpty() || ui->crPasswd->text().isEmpty())
    {
        QMessageBox::critical(this,tr("Невірний ввід"),tr("Заповніть всі поля"));
        return;
    }

    if (ui->crPasswd->text() != ui->crPasswd2->text())
    {
        QMessageBox::critical(this,tr("Невірний ввід"),tr("Введені паролі не співпадають"));
        ui->crPasswd->setFocus();
        return;
    }

    QString strPatt = "\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b";
    QRegExp rx("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b", Qt::CaseInsensitive);
    if(!rx.exactMatch(ui->crEmail->text()))
    {
        QMessageBox::critical(this,tr("Невірний ввід"),tr("Направильна електронна пошта. \nПриклад правильної електронної пошти:\n    example@gmail.com"));
        ui->crEmail->setFocus();
        return;
    }

    QSqlQuery q;
    if(ui->crLogin->text() != uLogin)
    {
        q.prepare("select login from users where login = :login and deleted = 0");
        q.bindValue(":login", ui->crLogin->text());
        if(!q.exec())
        {
            throwDBError(q.lastError());
            return;
        }
        if(MainWindow::qSize(q) > 0)
        {
            QMessageBox::critical(this,tr("Невірний ввід"),tr("Такий логін вже існує"));
            ui->crLogin->setFocus();
            return;
        }
    }

    if(ui->crEmail->text() != uEmail)
    {
        q.prepare("select email from users where email = :email and deleted = 0");
        q.bindValue(":email", ui->crEmail->text());
        if(!q.exec())
        {
            throwDBError(q.lastError());
            return;
        }
        if(MainWindow::qSize(q) > 0)
        {
            QMessageBox::critical(this,tr("Невірний ввід"),tr("Такий e-mail вже зареестрований"));
            ui->crEmail->setFocus();
            return;
        }
    }

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(ui->crLogin->text().toLatin1() + ui->crPasswd->text().toLatin1());

    if(idUser == 0)
    {
        q.prepare(QString("INSERT INTO users (login, md5, username, email, createat%1)"
                        "VALUES (:login, :md5, :username, :email, %2%3)")
                        .arg((SelectDatabase::getDBLocation() == true)?", lastLogin":"",
                        (SelectDatabase::getDBLocation() == true)?"datetime('now','localtime')":"now()",
                        (SelectDatabase::getDBLocation() == true)?",datetime('now','localtime')":""));
        q.bindValue(":login", ui->crLogin->text());
        q.bindValue(":md5", md5.result().toHex());
        q.bindValue(":username", ui->crName->text());
        q.bindValue(":email", ui->crEmail->text());
    }
    else
    {
        q.prepare("UPDATE users SET login = :login, md5 = :md5, username = :username, email = :email WHERE id = :id");
        q.bindValue(":login", ui->crLogin->text());
        q.bindValue(":md5", md5.result().toHex());
        q.bindValue(":username", ui->crName->text());
        q.bindValue(":email", ui->crEmail->text());
        q.bindValue(":id", idUser);
    }
    if(q.exec())
    {
        QMessageBox::information(this,tr("Операція успішна"), (idUser == 0)?tr("Реєстрація пройшла успішно"):tr("Користувач успішно відредагований"));
        this->close();
    }
    else
    {
        throwDBError(q.lastError());
        return;
    }
}

void Register::throwDBError(QSqlError error)
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

void Register::recieveData(int id, QString login, QString username, QString email, QString wndTitle, QString btnText)
{
    idUser = id;
    uLogin = login;
    ui->crLogin->setText(login);
    uEmail = email;
    ui->crEmail->setText(email);
    ui->crName->setText(username);
    this->setWindowTitle(wndTitle);
    ui->regButton->setText(btnText);
}

