#include "addquestion.h"
#include "ui_addquestion.h"

AddQuestion::AddQuestion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddQuestion)
{
    ui->setupUi(this);

    this->setWhatsThis(tr("Форма, що слугує для додавання або редагування питань, а також містить список відповідей, які відносяться до поточного питання. \n\nДля того, щоб змінни набули чинності, та текст питання оновився у базі даних, достатньо закрити форму."));
    ui->textEdit->setWhatsThis(tr("Текстове поле, призначене для введення питання."));
    ui->tableWidget->setWhatsThis(tr("Інтерактивна таблиця, яка містить інформацію про відповіді, які відосяться до поточного питання. А саме: текст відповіді та відмітку про її правильність."));
    ui->addAnswerButton->setWhatsThis(tr("Натисніть для додавання нової відповіді."));
    ui->editButton->setWhatsThis(tr("Натисність для редагування обраної відповіді. \n\nЯкщо таблиця не містить відповідей, або відповідь не обрана, кнопка буде недоступною. Для активації кнопки потрібно додати відповідь, або обрати її із таблиці."));
    ui->deleteButton->setWhatsThis(tr("Натисність для видалення обраної відповіді. \n\nЯкщо таблиця не містить відповідей, або відповідь не обрана, кнопка буде недоступною. Для активації кнопки потрібно додати відповідь, або обрати її із таблиці."));

    setModal(true);
    id_question = 0;
    tblRow = 0;
    isOpenedFromQuestionaryList = false;
}

AddQuestion::~AddQuestion()
{
    delete ui;
}

bool AddQuestion::changeData()
{
    QSqlQuery query;
    query.prepare(QString("UPDATE questions SET text_question = \"%1\" %2 WHERE id_question = %3;")
                  .arg(ui->textEdit->toPlainText(), (SelectDatabase::getDBLocation() == true)?", date_edit = datetime('now','localtime')":"", QString::number(id_question)));
    qDebug() << SelectDatabase::getDBLocation();
    qDebug() << query.lastQuery();
    /*query.bindValue(":text", ui->textEdit->toPlainText());
    query.bindValue(":id", id_question);*/
    if(!query.exec())
    {
        throwDBError(query.lastError());
        return false;
    }
    return true;
}

bool AddQuestion::checkForCorrectAnswers()
{
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        if(ui->tableWidget->item(i,1)->text() == "Так")
            return true;
    return false;
}

void AddQuestion::updateTable()
{
    ui->tableWidget->clear();
    QSqlQuery q;
    int i = 0;
    q.prepare("select id_answer,text_answer,correct from answers where id_question = :id and deleted = 0");
    q.bindValue(":id", id_question);
    if(!q.exec())
    {
        throwDBError(q.lastError());
        return;
    }

    ui->tableWidget->setRowCount(MainWindow::qSize(q));
    setColumns();
    while(q.previous()) {}
    while(q.next())
    {
        for(int k = 1; k < q.record().count(); k++)
        {
            QString temp = "";
            int align = Qt::AlignVCenter;
            if(k == 1)
                temp = q.value(k).toString();
            else
            {
                temp = (q.value(k).toInt() == 1) ? tr("Так") : tr("Ні");
                align = Qt::AlignCenter;
            }
            QTableWidgetItem * itm = new QTableWidgetItem(temp);
            itm->setData(Qt::UserRole, q.value(0));
            itm->setTextAlignment(align);
            ui->tableWidget->setItem(i,k-1,itm);
        }
        ui->tableWidget->setRowHeight(i, 40);
        i++;
    }

    ui->tableWidget->setCurrentCell(0,0);
}

void AddQuestion::on_addAnswerButton_clicked()
{
    AddAnswer *wnd = new AddAnswer(this);
    connect(this,SIGNAL(sendDataToAddAnswer(QString,QString,QString,int,int,bool,bool)), wnd, SLOT(recievedData(QString,QString,QString,int,int,bool,bool)));
    emit (sendDataToAddAnswer("", tr("Дод&ати"), tr("Додавання відповідей"), 0, id_question, false, false));
    //wnd->show();
    if(!wnd->exec())
    {
        updateTable();
        delete wnd;
        if(!ui->editButton->isEnabled()) ui->editButton->setEnabled(true);
        if(!ui->deleteButton->isEnabled()) ui->deleteButton->setEnabled(true);
        ui->tableWidget->setCurrentCell(ui->tableWidget->rowCount()-1,0);
    }
}

void AddQuestion::on_deleteButton_clicked()
{
    if(QMessageBox::question(this, tr("Підтвердіть Ваш вибір"), tr("Ви дійсно бажаєте видалити відповідь?"), tr("Так"), tr("Ні")) == 1)
        return;

    int currRow = ui->tableWidget->currentRow();
    QSqlQuery query;
    query.prepare("update answers set deleted = 1 where id_answer = :id");
    query.bindValue(":id", ui->tableWidget->currentItem()->data(Qt::UserRole).toInt());
    if(query.exec())
    {
        if(ui->tableWidget->rowCount() > 1)
        {
            updateTable();

            if(currRow == 0)
                ui->tableWidget->setCurrentItem(ui->tableWidget->item(0,0));
            else
                ui->tableWidget->setCurrentItem(ui->tableWidget->item(currRow-1,0));
        }
        else
        {
            ui->tableWidget->clear();
            ui->tableWidget->setRowCount(0);
            setColumns();
            ui->deleteButton->setEnabled(false);
            ui->editButton->setEnabled(false);
        }
    }
    else
    {
        throwDBError(query.lastError());
        return;
    }

    QMessageBox::information(this, tr("Операція успішна"), tr("Видалення пройшло успішно!"), tr("Гаразд"));
}

void AddQuestion::on_editButton_clicked()
{

    QModelIndex index = ui->tableWidget->currentIndex();
    AddAnswer *wnd = new AddAnswer(this);
    connect(this,SIGNAL(sendDataToAddAnswer(QString,QString,QString,int,int,bool,bool)), wnd, SLOT(recievedData(QString,QString,QString,int,int,bool,bool)));
    emit (sendDataToAddAnswer(ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text(), tr("Редаг&увати"), tr("Редагування відповіді"),
                   ui->tableWidget->currentItem()->data(Qt::UserRole).toInt(), id_question,
                   (ui->tableWidget->item(ui->tableWidget->currentRow(), 1)->text() == "Так") ? true : false, true));
    //wnd->show();

    if(!wnd->exec())
    {
        updateTable();
        delete wnd;
        ui->tableWidget->setCurrentIndex(index);
    }
}

void AddQuestion::on_tableWidget_itemSelectionChanged()
{
    if(isOpenedFromQuestionaryList)
        return;

    if(ui->tableWidget->currentItem()->isSelected())
    {
        ui->editButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
    }
    else
    {
        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    }

    qDebug() << ui->tableWidget->currentItem()->text() << ui->tableWidget->currentItem()->data(Qt::UserRole);
}

void AddQuestion::recievedData(bool flag, int id_q, QString text_question, int row)
{
    _flag = flag;
    ui->textEdit->setText(text_question);
    id_question = id_q;
    tblRow = row;
    updateTable();
}

void AddQuestion::recievedDataFromQuestionaryList(int id_q, QString text_question, bool flag)
{
    ui->addAnswerButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->editButton->setEnabled(false);
    ui->textEdit->setText(text_question);
    ui->textEdit->setReadOnly(true);
    id_question = id_q;
    isOpenedFromQuestionaryList = flag;
    updateTable();
}

void AddQuestion::closeEvent(QCloseEvent *e)
{
    if(isOpenedFromQuestionaryList) //Якщо вікно було відкрите із форми QuestionaryInfo
        return;

    int t = 0;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        if(ui->tableWidget->item(i,1)->text() == tr("Так"))
            t++;;

    if(_flag) //Якщо була натиснута кнопка редагування (на попередній формі)
    {
        if(ui->textEdit->toPlainText().isEmpty())
        {
            QMessageBox::information(this, tr("Помилка заповнення"), tr("Введіть текст запитання"));
            e->ignore();
            return;
        }

        if(!checkForCorrectAnswers())
        {
            if(QMessageBox::question(this, tr("Підтвердіть Ваш вибір"), tr("Не знайдено жодної правильної відповіді. Ви впевнені, що бажаєте закрити це вікно?"), tr("Так"), tr("Ні")) == 1)
            {
                e->ignore();
                return;
            }
        }
        if(!changeData())
            return;

        emit sendDataToQuestionary(ui->textEdit->toPlainText(), ui->tableWidget->rowCount(), t, tblRow);
        emit sendDataToQuestions(ui->textEdit->toPlainText(), ui->tableWidget->rowCount(), t, tblRow, id_question);
        ui->textEdit->clear();
    }
    else  //В іншому ж випадку
    {
        if(ui->textEdit->toPlainText().isEmpty())
        {
            if(QMessageBox::question(this, tr("Помилка заповнення"), tr("Текстове поле пусте. Ви впевнені, що бажаєте закрити вікно?"), tr("Так"), tr("Ні")) == 1)
            {
                e->ignore();
                return;
            }
            else
            {
                QSqlQuery q;
                q.prepare("UPDATE questions SET deleted = 1 WHERE id_question = :id");
                q.bindValue(":id", id_question);
                if(!q.exec())
                {
                    throwDBError(q.lastError());
                    return;
                }
            }
        }
        else
        {
            if(!checkForCorrectAnswers())
            {
                if(QMessageBox::question(this, tr("Підтвердіть свій вибір"), tr("Не знайдено жодної правильної відповіді. Ви впевнені, що бажаєте закрити це вікно?"), tr("Так"), tr("Ні")) == 1)
                {
                    e->ignore();
                    return;
                }
            }
            changeData();
            emit sendDataToQuestions(ui->textEdit->toPlainText(), ui->tableWidget->rowCount(), t, tblRow+1, id_question);
            ui->textEdit->clear();
        }
    }
}

void AddQuestion::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    emit on_editButton_clicked();
}

void AddQuestion::setColumns()
{
    ui->tableWidget->setColumnCount(2);
    QStringList list;
    list << tr("Відповідь") << tr("Чи правильна");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void AddQuestion::throwDBError(QSqlError error)
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
