#include "questions.h"
#include "ui_questions.h"

Questions::Questions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Questions)
{
    ui->setupUi(this);

    this->setWhatsThis(tr("Форма, що слугує для перегляду, додавання, редагування та видалення питань, які відносяться до обраної теми."));
    ui->tableWidget->setWhatsThis(tr("Інтерактивна таблиця, що містить інформацію про питання для поточної теми.\n Записи таблиці можуть бути зафарбовані. Якщо колір зафарбовування червоний, це значить, що питання не має жодної правильної відповіді.\nЯкщо жовтий - питання має лише правильні відповіді. \nЯкщо колір білий (зафарбовування відсутнє), то питання правильне, тобто має щонайменше одну правильну і одну неправильну відповіді."));
    ui->addButton->setWhatsThis(tr("Натисніть для додавання нового питання із обраної теми. \n\nЯкщо ж тему не було обрано, що свідчить про те, що в базі даних не міститься жодної теми, або виникла проблема зі з'єднанням до бази даних, кнопка перебуватиме в неактивному стані."));
    ui->editButton->setWhatsThis(tr("Натисніть для редагування обраного питання. \n\nЯкщо таблиця не містить питань, або питання не обране, кнопка буде недоступною. Для активації кнопки потрібно додати питання, або обрати його із таблиці."));
    ui->deleteButton->setWhatsThis(tr("Натисніть для видалення обраного питання. \n\nЯкщо таблиця не містить питань, або питання не обране, кнопка буде недоступною. Для активації кнопки потрібно додати питання, або обрати його із таблиці."));
    ui->themeButton->setWhatsThis(tr("Натисність для відкриття вікна вибору теми."));
    ui->lblSubject->setWhatsThis(tr("Напис, в якому зазначається обраний предмет. \n\nУ разі відсутності з'єднання із базою даних або у випадку, коли не обрана тема, напис відображатиме відповідне повідомлення, виділене червоним кольором."));
    ui->lblSection->setWhatsThis(tr("Напис, в якому зазначається обраний розділ. \n\nУ разі відсутності з'єднання із базою даних або у випадку, коли не обрана тема, напис відображатиме відповідне повідомлення, виділене червоним кольором."));
    ui->lblTheme->setWhatsThis(tr("Напис, в якому зазначається обрана тема. \n\nУ разі відсутності з'єднання із базою даних або у випадку, коли не обрана тема, напис відображатиме відповідне повідомлення, виділене червоним кольором."));

    QSqlQuery q;
    q.prepare("SELECT themes.id_theme, subjects.name_subject, sections.name_section, themes.name_theme\
              FROM (themes INNER JOIN sections ON themes.id_section = sections.id_section) INNER JOIN subjects ON sections.id_subject = subjects.id_subject\
              WHERE (((themes.deleted)=0)) LIMIT 1");

    if(!q.exec())
    {
        ui->lblSubject->setText("Проблема зі з'єднанням");
        ui->lblSection->setText("Проблема зі з'єднанням");
        ui->lblTheme->setText("Проблема зі з'єднанням");
        throwDBError(q.lastError());
        return;
    }

    if(MainWindow::qSize(q) > 0)
    {
        q.first();
        id_theme = q.value(0).toInt();
        ui->lblSubject->setText(q.value(1).toString());
        ui->lblSubject->setToolTip(q.value(1).toString());
        ui->lblSection->setText(q.value(2).toString());
        ui->lblSection->setToolTip(q.value(2).toString());
        ui->lblTheme->setText(q.value(3).toString());
        ui->lblTheme->setToolTip(q.value(3).toString());
        ui->lblSubject->setStyleSheet("margin-bottom: 6;");
        ui->lblSection->setStyleSheet("margin-bottom: 6;");
        ui->lblTheme->setStyleSheet("margin-bottom: 6;");
        this->setWindowTitle(tr("Список питань до теми \"") + q.value(3).toString() + "\"");
        ui->themeButton->setText("Обрати тему");
        updateTable();
        ui->addButton->setEnabled(true);
    }
    else
    {
        id_theme = 0;
        ui->lblSubject->setText("Додайте нову тему");
        ui->lblSection->setText("Додайте нову тему");
        ui->lblTheme->setText("Додайте нову тему");
        ui->themeButton->setText("Додати нову тему");
        ui->addButton->setEnabled(false);
        this->setWindowTitle(tr("Список питань"));
    }  
    setColumns();
}

Questions::~Questions()
{
    delete ui;
}

void Questions::updateTable()
{
    ui->tableWidget->clear();
    QSqlQuery q;
    int i = 0;
    qDebug() << id_theme << ":id";
    q.prepare("select id_question,text_question,createdBy,date_created,date_edit from questions where id_theme = :id and deleted = 0");
    q.bindValue(":id", id_theme);
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
            QTableWidgetItem * itm;
            if(k > 2)
            {
                itm = new QTableWidgetItem(q.value(k).toDate().toString());
                itm->setData(Qt::UserRole, q.value(0));
                ui->tableWidget->setItem(i,k-1,itm);
            }
            else
            {
                itm = new QTableWidgetItem(q.value(k).toString());
                itm->setData(Qt::UserRole, q.value(0));
                ui->tableWidget->setItem(i,k-1,itm);
            }

        }
        paintRow(i, q.value(0).toInt());
        ui->tableWidget->setRowHeight(i, 40);
        i++;
    }
    ui->tableWidget->setCurrentCell(0,0);
}

void Questions::on_addButton_clicked()
{
    QSqlQuery query;
    int questionID;

    //Додавання пустого питання в БД
    query.prepare(QString("INSERT INTO questions (id_theme,text_question,createdBy,date_created%1) VALUES (%2, \"\", \"%3\", %4%5)")
                  .arg((SelectDatabase::getDBLocation() == true)?",date_edit":"",
                       QString::number(id_theme),
                       Login::getUsername(),
                       (SelectDatabase::getDBLocation() == true)?"datetime('now','localtime')":"now()",
                       (SelectDatabase::getDBLocation() == true)?",datetime('now','localtime')":""));
    qDebug() << query.lastQuery();
    if(!query.exec())
    {
        throwDBError(query.lastError());
        return;
    }

    if(!query.exec("select max(id_question) from questions"))
    {
        throwDBError(query.lastError());
        return;
    }

    query.first();
        questionID = query.value(0).toInt();

    AddQuestion *wnd = new AddQuestion(this);
    connect(this, SIGNAL(sendDataToQuestion(bool,int,QString,int)), wnd, SLOT(recievedData(bool,int,QString,int)));
    connect(wnd, SIGNAL(sendDataToQuestions(QString,int,int,int,int)), this, SLOT(recieveDataFromAddQuestion(QString,int,int,int,int)));
    emit(sendDataToQuestion(false, questionID, "", ui->tableWidget->rowCount()));

    if(!wnd->exec())
    {
        delete wnd;
        ui->tableWidget->setCurrentCell(ui->tableWidget->rowCount()-1,0);
    }
}

void Questions::on_editButton_clicked()
{
    int currRow = ui->tableWidget->currentRow();

    AddQuestion *wnd = new AddQuestion(this);
    connect(this, SIGNAL(sendDataToQuestion(bool,int,QString,int)), wnd, SLOT(recievedData(bool,int,QString,int)));
    connect(wnd, SIGNAL(sendDataToQuestions(QString,int,int,int,int)), this, SLOT(recieveDataFromAddQuestion(QString,int,int,int,int)));
    emit(sendDataToQuestion(true, ui->tableWidget->currentItem()->data(Qt::UserRole).toInt(),
                            ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text(),
                            ui->tableWidget->currentRow()));

    if(!wnd->exec())
    {
        delete wnd;
        ui->tableWidget->setCurrentCell(currRow,0);
    }
}

void Questions::on_deleteButton_clicked()
{
    if(QMessageBox::question(this, tr("Підтвердіть Ваш вибір"), tr("Ви впевнені, що хочете видалити цей запис?"), tr("Так"), tr("Ні")) == 1)
        return;

    int currRow = ui->tableWidget->currentRow();
    QSqlQuery query;
    query.prepare("update questions set deleted = 1 where id_question = :id");
    query.bindValue(":id", ui->tableWidget->currentItem()->data(Qt::UserRole).toInt());
    if(!query.exec())
    {
        throwDBError(query.lastError());
        return;
    }

    query.prepare("update answers set deleted = 1 where id_question = :id");
    query.bindValue(":id", ui->tableWidget->currentItem()->data(Qt::UserRole).toInt());
    if(!query.exec())
    {
        throwDBError(query.lastError());
        return;
    }

    if(ui->tableWidget->rowCount() > 1)
    {
        updateTable();

        if(currRow == 0)
            ui->tableWidget->setCurrentCell(0,0);
        else
            ui->tableWidget->setCurrentCell(currRow-1,0);
    }
    else
    {
        ui->tableWidget->clear();
        ui->tableWidget->setRowCount(0);

        setColumns();

        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    }
    QMessageBox::information(this, tr("Операція успішна"), tr("Видалення пройшло успішно!"), tr("Гаразд"));
}

void Questions::on_tableWidget_itemSelectionChanged()
{
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
}

void Questions::on_tableWidget_cellDoubleClicked(int row, int column)
{
    emit on_editButton_clicked();
}

void Questions::recieveDataFromThemes(int id_thme, QString name_subject, QString name_section, QString name_theme, bool themeBtn)
{
    ui->themeButton->setEnabled(themeBtn);
    setModal(!themeBtn);
    id_theme = id_thme;
    if(id_theme == -1)
    {
        ui->addButton->setEnabled(false);
        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        ui->lblSubject->setStyleSheet("margin-bottom: 6; color: red;");
        ui->lblSection->setStyleSheet("margin-bottom: 6; color: red;");
        ui->lblTheme->setStyleSheet("margin-bottom: 6; color: red;");
        this->setWindowTitle(tr("Список питань"));
    }
    else
    {
        ui->addButton->setEnabled(true);
        ui->lblSubject->setStyleSheet("margin-bottom: 6;");
        ui->lblSection->setStyleSheet("margin-bottom: 6;");
        ui->lblTheme->setStyleSheet("margin-bottom: 6;");
        this->setWindowTitle(tr("Список питань до теми \"") + name_theme + "\"");
    }
    ui->lblSubject->setText(name_subject);
    ui->lblSubject->setToolTip(name_subject);
    ui->lblSection->setText(name_section);
    ui->lblSection->setToolTip(name_section);
    ui->lblTheme->setText(name_theme);
    ui->lblTheme->setToolTip(name_theme);
    updateTable();
}

void Questions::recieveDataFromAddQuestion(QString text, int aCount, int correctCount, int row, int qID)
{
    QColor color;
    if(correctCount == 0) //Якщо немає правильних відповідей
        color.setRgb(240, 92, 92); //Червоний
    else if(correctCount == aCount) //Якщо кількість правильних відповідей == загальній кількості відповідей
        color.setRgb(242, 215, 62); //Жовтий
    else //Якщо все добре
        color.setRgb(255,255,255);

    QSqlQuery q;
    q.prepare(QString("SELECT %1").arg((SelectDatabase::getDBLocation()==true)?"datetime('now','localtime')":"now()"));
    if(!q.exec())
    {
        throwDBError(q.lastError());
        return;
    }
    q.first();

    if(row > ui->tableWidget->rowCount()) //Якщо була натиснута кнопка додати
    {
        ui->tableWidget->setRowCount(row);
        QTableWidgetItem *itm1 = new QTableWidgetItem(text);
        itm1->setData(Qt::UserRole, qID);
        ui->tableWidget->setItem(row-1,0,itm1);

        QTableWidgetItem *itm2 = new QTableWidgetItem(Login::getUsername());
        itm2->setData(Qt::UserRole, qID);
        ui->tableWidget->setItem(row-1,1,itm2);

        QTableWidgetItem *itm3 = new QTableWidgetItem(q.value(0).toDate().toString());
        itm3->setData(Qt::UserRole, qID);
        ui->tableWidget->setItem(row-1,2,itm3);

        QTableWidgetItem *itm4 = new QTableWidgetItem(q.value(0).toDate().toString());
        itm4->setData(Qt::UserRole, qID);
        ui->tableWidget->setItem(row-1,3,itm4);

        for(int i = 0; i < ui->tableWidget->columnCount(); i++)
            ui->tableWidget->item(row-1, i)->setBackgroundColor(color);

        ui->tableWidget->setRowHeight(row-1, 40);
    }
    else
    {
        ui->tableWidget->item(row,0)->setText(text);
        ui->tableWidget->item(row,3)->setText(q.value(0).toDate().toString());

        for(int i = 0; i < ui->tableWidget->columnCount(); i++)
            ui->tableWidget->item(row, i)->setBackgroundColor(color);
    }
}

void Questions::on_themeButton_clicked()
{
    /*HelpSubjects *wnd = new HelpSubjects();
    connect(this, SIGNAL(sendDataToThemes(bool)), wnd, SLOT(recieveData(bool)));
    connect(wnd, SIGNAL(sendData(int,QString,QString,QString,bool)),this,SLOT(recieveDataFromThemes(int,QString,QString,QString,bool)));
    emit sendDataToThemes(true);
    //wnd->show();
    wnd->exec();
    //if(!wnd->exec())
        //delete wnd;*/
    HelpSubjects *wnd = new HelpSubjects();
    connect(this, SIGNAL(sendDataToThemes(bool)), wnd, SLOT(recieveData(bool)));
    connect(wnd, SIGNAL(sendData(int,QString,QString,QString,bool)),this,SLOT(recieveDataFromThemes(int,QString,QString,QString,bool)));
    emit sendDataToThemes(true);
    wnd->show();
    if(!wnd->exec())
        delete wnd;
}

void Questions::setColumns()
{
    ui->tableWidget->setColumnCount(4);
    QStringList list;
    list << tr("Текст запитання") << tr("Ким створене") << tr("Дата створення") << tr("Дата редагування");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setColumnWidth(2, 125);
    ui->tableWidget->setColumnWidth(3, 125);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
}

void Questions::throwDBError(QSqlError error)
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

void Questions::paintRow(int row, int id_question)
{
    int count = 0;
    int rightCount = 0;
    QColor color;
    QSqlQuery qq;
    qq.prepare("SELECT count(id_answer) FROM answers WHERE id_question = :id AND deleted = 0");
    qq.bindValue(":id", id_question);
    if(!qq.exec())
    {
        throwDBError(qq.lastError());
        return;
    }
    qq.first();
    count = qq.value(0).toInt();

    qq.prepare("SELECT count(id_answer) FROM answers WHERE id_question = :id AND deleted = 0 AND correct = 1");
    qq.bindValue(":id", id_question);
    if(!qq.exec())
    {
        throwDBError(qq.lastError());
        return;
    }
    qq.first();
    rightCount = qq.value(0).toInt();

    if(rightCount == 0) //Якщо немає правильних відповідей
        color.setRgb(240, 92, 92); //Червоний
    else if(rightCount == count) //Якщо кількість правильних відповідей == загальній кількості відповідей
        color.setRgb(242, 215, 62); //Жовтий
    else //Якщо все добре
        color.setRgb(255,255,255);

    for(int i = 0; i < ui->tableWidget->columnCount(); i++)
        ui->tableWidget->item(row, i)->setBackgroundColor(color);
}
