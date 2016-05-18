#include "questionarylist.h"
#include "ui_questionarylist.h"

QuestionaryList::QuestionaryList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuestionaryList)
{
    ui->setupUi(this);

    this->setWhatsThis(tr("Форма, яка містить список створених користувачем тестувальників. Також є можливість додати новий тестувальник, видалити існуючий, переглянути більш детальну інформацію по тестувальнику, та перейти до друку"));
    ui->themeButton->setWhatsThis(tr("Натисність для відкриття вікна вибору теми."));
    ui->lblSubject->setWhatsThis(tr("Напис, в якому зазначається обраний предмет. \n\nУ разі відсутності з'єднання із базою даних або у випадку, коли не обрана тема, напис відображатиме відповідне повідомлення, виділене червоним кольором."));
    ui->lblSection->setWhatsThis(tr("Напис, в якому зазначається обраний розділ. \n\nУ разі відсутності з'єднання із базою даних або у випадку, коли не обрана тема, напис відображатиме відповідне повідомлення, виділене червоним кольором."));
    ui->lblTheme->setWhatsThis(tr("Напис, в якому зазначається обрана тема. \n\nУ разі відсутності з'єднання із базою даних або у випадку, коли не обрана тема, напис відображатиме відповідне повідомлення, виділене червоним кольором."));
    ui->addButton->setWhatsThis(tr("Натисніть для додавання нового тестувальника."));
    ui->deleteButton->setWhatsThis(tr("Натисніть для видалення обраного тестувальника."));
    ui->infoButton->setWhatsThis(tr("Натисніть для перегляду додаткової інформації по обраному тестувальнику."));
    ui->printButton->setWhatsThis(tr("Натисніть для друку обраного тестувальника."));
    ui->closeButton->setWhatsThis(tr("Натисніть для закриття форми."));
    ui->tableWidget->setWhatsThis(tr("Інтерактивна таблиця, що містить інформацію про тестувальники до поточної теми."));

    QSqlQuery q;
    q.prepare("SELECT themes.id_theme, subjects.name_subject, sections.name_section, themes.name_theme\
              FROM (themes INNER JOIN sections ON themes.id_section = sections.id_section) INNER JOIN subjects ON sections.id_subject = subjects.id_subject\
              WHERE (((themes.deleted)=0)) LIMIT 1");

    if(!q.exec())
    {
        ui->lblSubject->setText("Проблема зі з'єднанням");
        ui->lblSection->setText("Проблема зі з'єднанням");
        ui->lblTheme->setText("Проблема зі з'єднанням");
        ui->addButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        ui->infoButton->setEnabled(false);
        ui->themeButton->setEnabled(false);
        throwDBError(q.lastError());
        return;
    }

    if(MainWindow::qSize(q) > 0)
    {
        q.first();
        id_theme = q.value(0).toInt();
        ui->lblSubject->setText(q.value(1).toString());
        ui->lblSection->setToolTip(q.value(2).toString());
        ui->lblSection->setText(q.value(2).toString());
        ui->lblSection->setToolTip(q.value(2).toString());
        ui->lblTheme->setText(q.value(3).toString());
        ui->lblTheme->setToolTip(q.value(3).toString());
        ui->lblSubject->setStyleSheet("margin-bottom: 6;");
        ui->lblSection->setStyleSheet("margin-bottom: 6;");
        ui->lblTheme->setStyleSheet("margin-bottom: 6;");
        this->setWindowTitle(tr("Список тестувальників до теми \"") + q.value(3).toString() + "\"");
        ui->themeButton->setText("Обрати тему");
        updateTable();
    }
    else
    {
        id_theme = 0;
        ui->deleteButton->setEnabled(false);
        ui->infoButton->setEnabled(false);
        ui->addButton->setEnabled(false);
        ui->lblSubject->setText("Додайте нову тему");
        ui->lblSection->setText("Додайте нову тему");
        ui->lblTheme->setText("Додайте нову тему");
        ui->themeButton->setText("Додати нову тему");
        this->setWindowTitle(tr("Список тестувальників"));
    }
    ui->tableWidget->setColumnCount(4);
    QStringList list;
    list << tr("Назва тестувальника") << tr("К-сть питань") << tr("Ким створений") << tr("Дата створення");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->setColumnWidth(3,125);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
}

QuestionaryList::~QuestionaryList()
{
    delete ui;
}

void QuestionaryList::updateTable()
{
    ui->tableWidget->clear();
    int i = 0;
    QSqlQuery q;
    q.prepare("select id_questionary, name_questionary, createdBy, date_created from questionary where id_theme = :id");
    q.bindValue(":id", id_theme);
    if(!q.exec())
    {
        throwDBError(q.lastError());
        return;
    }

    int count = MainWindow::qSize(q);
    if(count < 0) count = 0;
    if(count == 0)
    {
        ui->deleteButton->setEnabled(false);
        ui->infoButton->setEnabled(false);
    }
    else
    {
        ui->deleteButton->setEnabled(true);
        ui->infoButton->setEnabled(true);
    }

    ui->tableWidget->setRowCount(count);
    ui->tableWidget->setColumnCount(4);
    QStringList list;
    list << tr("Назва тестувальника") << tr("К-сть питань") << tr("Ким створений") << tr("Дата створення");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->setColumnWidth(3,125);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    while(q.previous()) {}

    while(q.next())
    {
        QTableWidgetItem *itm1 = new QTableWidgetItem(q.value(1).toString());
        itm1->setData(Qt::UserRole,q.value(0).toInt());
        itm1->setToolTip(q.value(1).toString());
        ui->tableWidget->setItem(i,0,itm1);

        QSqlQuery qq;
        qq.prepare("SELECT count(id_question) FROM questions_questionary WHERE id_questionary = :id");
        qq.bindValue(":id", q.value(0).toInt());
        if(!qq.exec())
        {
            ui->tableWidget->clear();
            ui->tableWidget->setRowCount(0);
            throwDBError(qq.lastError());
            return;
        }
        qq.first();
        count = qq.value(0).toInt();

        QTableWidgetItem *itm2 = new QTableWidgetItem(QString::number(count));
        itm2->setData(Qt::UserRole,q.value(0).toInt());
        itm2->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,1,itm2);

        QTableWidgetItem *itm3 = new QTableWidgetItem(q.value(2).toString());
        itm3->setData(Qt::UserRole,q.value(0).toInt());
        ui->tableWidget->setItem(i,2,itm3);

        QTableWidgetItem *itm4 = new QTableWidgetItem(q.value(3).toDate().toString());
        itm4->setData(Qt::UserRole,q.value(0).toInt());
        ui->tableWidget->setItem(i,3,itm4);

        ui->tableWidget->setRowHeight(i, 40);
        i++;
    }
    ui->tableWidget->setCurrentCell(0,0);
}

void QuestionaryList::on_infoButton_clicked()
{
    QuestionaryInfo *wnd = new QuestionaryInfo();
    connect(this, SIGNAL(sendDataToQuestionaryInfo(int,QString,QString,QString,QString,QString)),
            wnd, SLOT(recieveDataFromQuestionaryList(int,QString,QString,QString,QString,QString)));
    emit sendDataToQuestionaryInfo(ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->data(Qt::UserRole).toInt(),
                                   ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text(),
                                   ui->lblTheme->text(),
                                   ui->tableWidget->item(ui->tableWidget->currentRow(), 1)->text(),
                                   ui->tableWidget->item(ui->tableWidget->currentRow(), 3)->text(),
                                   ui->tableWidget->item(ui->tableWidget->currentRow(), 2)->text());
    wnd->show();
    if(!wnd->exec())
        delete wnd;
}

void QuestionaryList::on_themeButton_clicked()
{
    HelpSubjects *wnd = new HelpSubjects();
    connect(this, SIGNAL(sendDataToThemes(bool)), wnd, SLOT(recieveData(bool)));
    connect(wnd, SIGNAL(sendData(int,QString,QString,QString,bool)),this,SLOT(recieveDataFromThemes(int,QString,QString,QString)));
    emit sendDataToThemes(true);
    wnd->show();
    if(!wnd->exec())
        delete wnd;
}

void QuestionaryList::recieveDataFromThemes(int id_thme, QString name_subject, QString name_section, QString name_theme)
{
    id_theme = id_thme;
    if(id_theme == -1)
    {
        ui->addButton->setEnabled(false);
        ui->lblSubject->setStyleSheet("margin-bottom: 6; color: red;");
        ui->lblSection->setStyleSheet("margin-bottom: 6; color: red;");
        ui->lblTheme->setStyleSheet("margin-bottom: 6; color: red;");
        this->setWindowTitle(tr("Список тестувальників"));
    }
    else
    {
        ui->addButton->setEnabled(true);
        ui->lblSubject->setStyleSheet("margin-bottom: 6;");
        ui->lblSection->setStyleSheet("margin-bottom: 6;");
        ui->lblTheme->setStyleSheet("margin-bottom: 6;");
        this->setWindowTitle(tr("Список тестувальників до теми \"") + name_theme + "\"");
    }
    ui->lblSubject->setText(name_subject);
    ui->lblSubject->setToolTip(name_subject);
    ui->lblSection->setText(name_section);
    ui->lblSection->setToolTip(name_section);
    ui->lblTheme->setText(name_theme);
    ui->lblTheme->setToolTip(name_theme);
    updateTable();
}

void QuestionaryList::on_tableWidget_itemSelectionChanged()
{
    if(ui->tableWidget->currentItem()->isSelected())
    {
        ui->infoButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->printButton->setEnabled(true);
    }
    else
    {
        ui->infoButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        ui->printButton->setEnabled(false);
    }
    qDebug() << "ID = " << ui->tableWidget->currentItem()->data(Qt::UserRole);
}

void QuestionaryList::on_closeButton_clicked()
{
    this->close();
}

void QuestionaryList::on_deleteButton_clicked()
{
    if(QMessageBox::question(this, tr("Підтвердіть Ваш вибір"), tr("Ви впевнені, що хочете видалити цей тестувальник?"), tr("Так"), tr("Ні")) == 1)
        return;

    QSqlQuery q;
    q.prepare("DELETE FROM questions_questionary WHERE id_questionary = :id");
    q.bindValue(":id", ui->tableWidget->currentItem()->data(Qt::UserRole).toInt());
    if(!q.exec())
    {
        throwDBError(q.lastError());
        return;
    }

    q.prepare("DELETE FROM questionary WHERE id_questionary = :id");
    q.bindValue(":id", ui->tableWidget->currentItem()->data(Qt::UserRole).toInt());
    if(!q.exec())
    {
        throwDBError(q.lastError());
        return;
    }

    updateTable();
    QMessageBox::information(this, tr("Операція успішна"), tr("Тестувальник успішно видалений"), tr("Гаразд"));
}

void QuestionaryList::on_addButton_clicked()
{
    Questionary *wnd = new Questionary();
    connect(this, SIGNAL(sendDataToQuestionary(int, QString, QString, QString, bool)), wnd, SLOT(recieveDataFromQuestionaryList(int, QString, QString, QString, bool)));
    emit sendDataToQuestionary(id_theme,
                  ui->lblSubject->text(),
                  ui->lblSection->text(),
                  ui->lblTheme->text(),
                  false);
    wnd->show();
    if(!wnd->exec())
    {
        delete wnd;
        updateTable();
    }
}

void QuestionaryList::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    emit on_infoButton_clicked();
}

void QuestionaryList::throwDBError(QSqlError error)
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

void QuestionaryList::on_printButton_clicked()
{
    PrintQuestionary *wnd = new PrintQuestionary(this);
    connect(this, SIGNAL(sendDataToPrint(int,QString,QString,QString)), wnd, SLOT(recieveDataFromQuestionaryList(int,QString,QString,QString)));
    emit sendDataToPrint(ui->tableWidget->currentItem()->data(Qt::UserRole).toInt(),
                         ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text(),
                         ui->lblSubject->text(),
                         ui->lblTheme->text());
    wnd->show();
    if(!wnd->exec())
        delete wnd;
}
