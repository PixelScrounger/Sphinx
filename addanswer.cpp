#include "addanswer.h"
#include "ui_addanswer.h"

AddAnswer::AddAnswer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAnswer)
{
    ui->setupUi(this);
    setModal(true);

    this->setWhatsThis(tr("Форма, що слугує для додавання та редагування відповідей."));
    ui->textEdit->setWhatsThis("Текстове поле, призначене для введення відповіді.");
    ui->checkBox->setWhatsThis(tr("Прапорець, який відзначає правильність відповіді."));
    ui->addButton->setWhatsThis(tr("Натисніть для додавання або редагування (відносно контексту) відповіді. \n\nЯкщо текстове поле пусте, то кнопка перебуватиме у неактивному стані. Для активізації кнопки варто заповнити текстове поле."));
    ui->cancelButton->setWhatsThis(tr("Натисніть для закриття вікна."));

    connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(enableButton()));
    idA = 0;
    idQ = 0;
}

AddAnswer::~AddAnswer()
{
    delete ui;
}

void AddAnswer::on_cancelButton_clicked()
{
    this->close();
}

void AddAnswer::enableButton()
{
    if(ui->textEdit->toPlainText().isEmpty())
        ui->addButton->setEnabled(false);
    else
        ui->addButton->setEnabled(true);
}

void AddAnswer::on_addButton_clicked()
{
    qint8 checkBoxStatus = 0;
    if(ui->checkBox->isChecked())
        checkBoxStatus = 1;
    qDebug() << checkBoxStatus;

    if(_flag)
    {
        QSqlQuery query;
        query.prepare("UPDATE answers SET text_answer = :text, correct = :correct WHERE id_answer = :idA");
        query.bindValue(":text", ui->textEdit->toPlainText());
        query.bindValue(":correct", checkBoxStatus);
        query.bindValue(":idA", idA);
        if(query.exec())
        {
            if(QMessageBox::question(this, tr("Операція успішна"), tr("Відповідь успішно відредаговано.\nБажаєте закрити вікно?"), tr("Так"), tr("Ні")) == 0)
                this->close();
        }
        else
        {
            throwDBError(query.lastError());
            return;
        }
    }
    else
    {
        QSqlQuery query;
        query.prepare("insert into answers (id_question,text_answer,correct) values (:id,:text,:correct)");
        query.bindValue(":id", idQ);
        query.bindValue(":text", ui->textEdit->toPlainText());
        query.bindValue(":correct", checkBoxStatus);
        if(query.exec())
        {
            if(QMessageBox::question(this, tr("Операція успішна"), tr("Відповідь успішно додано.\nБажаєте додати ще одну?"), tr("Так"), tr("Ні")) == 0)
            {
                ui->checkBox->setChecked(false);
                ui->textEdit->setText("");
                ui->textEdit->setFocus();
            }
            else
                this->close();
        }
        else
        {
            throwDBError(query.lastError());
            return;
        }
    }
}

void AddAnswer::throwDBError(QSqlError error)
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

void AddAnswer::recievedData(QString text, QString buttonLabel, QString windowLabel, int idAnswer, int idQuestion, bool correct, bool flag)
{
    ui->textEdit->setText(text);
    _flag = flag;
    idQ = idQuestion;
    idA = idAnswer;
    ui->checkBox->setChecked(correct);
    ui->addButton->setText(buttonLabel);
    this->setWindowTitle(windowLabel);
}
