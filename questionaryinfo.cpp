#include "questionaryinfo.h"
#include "ui_questionaryinfo.h"

QuestionaryInfo::QuestionaryInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuestionaryInfo)
{
    ui->setupUi(this);

    setModal(true);
    this->setWhatsThis(tr("Форма, що містить детальний опис одного тестувальника."));
    ui->name->setWhatsThis(tr("Напис, що вказує ім'я тестувальника."));
    ui->date->setWhatsThis(tr("Напис, що вказує дату створення тестувальника."));
    ui->creator->setWhatsThis(tr("Напис, що вказує ім'я користувача, який створив цей тестувальник."));
    ui->theme->setWhatsThis(tr("Напис, що вказує тему, якої стосується цей тестувальник."));
    ui->qCount->setWhatsThis(tr("Напис, що вказує кількість питань, які містяться в тестувальнику."));
    ui->tableWidget->setWhatsThis(tr("Інтерактивна таблиця, яка містить інформацію про питання, які містяться в тестувальнику."));
    ui->closeButton->setWhatsThis(tr("Натисніть щоб закрити форму"));
    ui->infoButton->setWhatsThis(tr("Натисніть для детальнішого перегляду інформації про обране питання."));
}

QuestionaryInfo::~QuestionaryInfo()
{
    delete ui;
}

void QuestionaryInfo::recieveDataFromQuestionary(QString name, QString theme, QString qCount, QVector<QString> questions, QVector<QString> answersCount, QVector<int> id)
{
    ui->name->setText(name);
    ui->theme->setText(theme);
    ui->creator->setText(Login::getUsername());
    ui->date->setText(QDate::currentDate().toString());
    ui->qCount->setText(qCount);
    this->setWindowTitle(tr("Інформація про тестувальник \"") + name + "\"");

    ui->tableWidget->setRowCount(questions.size());
    ui->tableWidget->setColumnCount(2);

    QStringList list;
    list << tr("Текст запитання") << tr("К-сть відповідей");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    for(int i = 0; i < questions.size(); i++)
    {
        QTableWidgetItem *itm1 = new QTableWidgetItem(questions[i]);
        itm1->setToolTip(questions[i]);
        itm1->setData(Qt::UserRole,id[i]);
        ui->tableWidget->setItem(i,0,itm1);

        QTableWidgetItem *itm2 = new QTableWidgetItem(answersCount[i]);
        itm2->setTextAlignment(Qt::AlignCenter);
        itm2->setData(Qt::UserRole,id[i]);
        ui->tableWidget->setItem(i,1,itm2);

        ui->tableWidget->setRowHeight(i, 40);
    }
    ui->tableWidget->setCurrentCell(0,0);
}

void QuestionaryInfo::recieveDataFromQuestionaryList(int id_questionary, QString name, QString theme, QString qCount, QString date, QString creator)
{
    ui->name->setText(name);
    ui->theme->setText(theme);
    ui->creator->setText(creator);
    ui->date->setText(date);
    ui->qCount->setText(qCount);
    this->setWindowTitle(tr("Інформація про тестувальник \"") + name + "\"");

    QSqlQuery q;
    QSqlQuery qq;
    q.prepare("SELECT id_question FROM questions_questionary WHERE id_questionary = :id");
    q.bindValue(":id", id_questionary);
    if(!q.exec())
    {
        throwDBError(q.lastError());
        return;
    }

    ui->tableWidget->setRowCount(MainWindow::qSize(q));
    ui->tableWidget->setColumnCount(2);

    QStringList list;
    list << tr("Текст запитання") << tr("К-сть відповідей");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    while(q.previous()) {}
    int i = 0;
    while(q.next())
    {
        qq.prepare("SELECT text_question FROM questions WHERE id_question = :id");
        qq.bindValue(":id", q.value(0).toInt());
        if(!qq.exec())
        {
            throwDBError(qq.lastError());
            return;
        }
        qq.first();
        QTableWidgetItem *itm1 = new QTableWidgetItem(qq.value(0).toString());
        itm1->setToolTip(qq.value(0).toString());
        itm1->setData(Qt::UserRole, q.value(0).toInt());
        ui->tableWidget->setItem(i,0,itm1);

        qq.prepare("SELECT Count(id_answer) FROM answers WHERE id_question = :id AND deleted = 0");
        qq.bindValue(":id", q.value(0).toInt());
        if(!qq.exec())
        {
            throwDBError(qq.lastError());
            return;
        }
        qq.first();
        QTableWidgetItem *itm2 = new QTableWidgetItem(qq.value(0).toString());
        itm2->setTextAlignment(Qt::AlignCenter);
        itm2->setData(Qt::UserRole, q.value(0).toInt());
        ui->tableWidget->setItem(i,1,itm2);

        ui->tableWidget->setRowHeight(i, 40);
        i++;
    }
    ui->tableWidget->setCurrentCell(0,0);
}

void QuestionaryInfo::on_closeButton_clicked()
{
    this->close();
}

void QuestionaryInfo::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    emit on_infoButton_clicked();
}

void QuestionaryInfo::on_infoButton_clicked()
{
    AddQuestion *wnd = new AddQuestion();
    connect(this, SIGNAL(sendDataToAddQuestion(int,QString,bool)), wnd, SLOT(recievedDataFromQuestionaryList(int,QString,bool)));
    emit sendDataToAddQuestion(ui->tableWidget->currentItem()->data(Qt::UserRole).toInt(),
                               ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text(),
                               true);
    wnd->show();
    if(!wnd->exec())
    {
        delete wnd;
        ui->tableWidget->setFocus();
    }
}

void QuestionaryInfo::on_tableWidget_itemSelectionChanged()
{
    if(ui->tableWidget->currentItem()->isSelected())
        ui->infoButton->setEnabled(true);
    else
        ui->infoButton->setEnabled(false);
}

void QuestionaryInfo::throwDBError(QSqlError error)
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
