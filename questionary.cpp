#include "questionary.h"
#include "ui_questionary.h"

Questionary::Questionary(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Questionary)
{
    ui->setupUi(this);

    this->setWhatsThis(tr("Форма, що слугує для створення тестувальників."));
    ui->themeButton->setWhatsThis(tr("Натисність для відкриття вікна вибору теми."));
    ui->lblSubject->setWhatsThis(tr("Напис, в якому зазначається обраний предмет. \n\nУ разі відсутності з'єднання із базою даних або у випадку, коли не обрана тема, напис відображатиме відповідне повідомлення, виділене червоним кольором."));
    ui->lblSection->setWhatsThis(tr("Напис, в якому зазначається обраний розділ. \n\nУ разі відсутності з'єднання із базою даних або у випадку, коли не обрана тема, напис відображатиме відповідне повідомлення, виділене червоним кольором."));
    ui->lblTheme->setWhatsThis(tr("Напис, в якому зазначається обрана тема. \n\nУ разі відсутності з'єднання із базою даних або у випадку, коли не обрана тема, напис відображатиме відповідне повідомлення, виділене червоним кольором."));
    ui->editButton->setWhatsThis(tr("Натисніть для відкритя форми редагування питань для поточної теми."));
    ui->randomButton->setWhatsThis(tr("Натисніть для вибору випадкових питань. Кількість випадкових питань вказується у полі праворуч."));
    ui->tableWidget->setWhatsThis(tr("Інтерактивна таблиця, що містить інформацію про питання для поточної теми. Має функцію множинного вибору. Якщо питання виділене, то воно добавиться до тестувальника.\nЗаписи таблиці можуть бути зафарбовані. Якщо колір зафарбовування червоний, це значить, що питання не має жодної правильної відповіді.\nЯкщо жовтий - питання має лише правильні відповіді. \nЯкщо колір білий (зафарбовування відсутнє), то питання правильне, тобто має щонайменше одну правильну і одну неправильну відповіді."));
    ui->nameLine->setWhatsThis(tr("Поле, у якому заповнюється назва тестувальника."));
    ui->addButton->setWhatsThis(tr("Натисніть для додавання нового тестувальника у базу даних. Список питань формується на основі обраних записів із таблиці зліва."));

    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(checkLabelGrammar()));

    QSqlQuery q;
    q.prepare("SELECT themes.id_theme, subjects.name_subject, sections.name_section, themes.name_theme\
              FROM (themes INNER JOIN sections ON themes.id_section = sections.id_section) INNER JOIN subjects ON sections.id_subject = subjects.id_subject\
              WHERE (((themes.deleted)=0)) LIMIT 1");

    if(!q.exec())
    {
        ui->lblSubject->setText("Проблема зі з'єднанням");
        ui->lblSection->setText("Проблема зі з'єднанням");
        ui->lblTheme->setText("Проблема зі з'єднанням");
        ui->themeButton->setEnabled(false);
        ui->addButton->setEnabled(false);
        ui->editButton->setEnabled(false);
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
        this->setWindowTitle(tr("Створення тестувальника до теми \"") + q.value(3).toString() + "\"");
        ui->themeButton->setText("Обрати тему");
        fillNameLine(q.value(3).toString());
        ui->nameLine->setStyleSheet("");
        ui->nameLine->setReadOnly(false);
        updateTable();
    }
    else
    {
        id_theme = 0;
        ui->editButton->setEnabled(false);
        ui->randomButton->setEnabled(false);
        ui->addButton->setEnabled(false);
        ui->lblSubject->setText("Додайте нову тему");
        ui->lblSection->setText("Додайте нову тему");
        ui->lblTheme->setText("Додайте нову тему");
        ui->themeButton->setText("Додати нову тему");
        this->setWindowTitle(tr("Створення тестувальника"));
        ui->nameLine->setText(tr("Додайте нову тему"));
        ui->nameLine->setStyleSheet("color: red;");
        ui->nameLine->setReadOnly(true);
    }
    ui->tableWidget->setColumnCount(2);
    QStringList list;
    list << tr("Текст запитання") << tr("К-сть відповідей");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

Questionary::~Questionary()
{
    delete ui;
}

void Questionary::updateTable()
{
    ui->tableWidget->clear();
    int i = 0;
    QSqlQuery q;
    q.prepare("select id_question, text_question from questions where id_theme = :id and deleted = 0");
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
        ui->randomButton->setEnabled(false);
        ui->addButton->setEnabled(false);
        ui->spinBox->setValue(1);
    }
    else
    {
        ui->randomButton->setEnabled(true);
        ui->addButton->setEnabled(true);
        ui->spinBox->setMaximum((count>100)?100:count);
    }

    ui->tableWidget->setRowCount(count);
    ui->tableWidget->setColumnCount(2);
    QStringList list;
    list << tr("Текст запитання") << tr("К-сть відповідей");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    while(q.previous()) {}

    while(q.next())
    {
        QTableWidgetItem *itm1 = new QTableWidgetItem(q.value(1).toString());
        itm1->setData(Qt::UserRole,q.value(0).toInt());
        itm1->setToolTip(q.value(1).toString());
        ui->tableWidget->setItem(i,0,itm1);

        QSqlQuery qq;
        qq.prepare("SELECT count(id_answer) FROM answers WHERE id_question = :id AND deleted = 0");
        qq.bindValue(":id", q.value(0).toInt());
        if(!qq.exec())
        {
            ui->tableWidget->clear();
            ui->tableWidget->setRowCount(0);
            ui->tableWidget->setColumnCount(0);
            throwDBError(qq.lastError());
            return;
        }
        qq.first();

        count = qq.value(0).toInt();
        QTableWidgetItem *itm2 = new QTableWidgetItem(QString::number(count));
        itm2->setData(Qt::UserRole,q.value(0).toInt());
        itm2->setToolTip(q.value(1).toString());
        if(count == 0)
            itm2->setFont(QFont("Segoe UI", 9, QFont::Bold));
        else
            itm2->setFont(QFont("Segoe UI", 9, QFont::Normal));
        itm2->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,1,itm2);

        qq.prepare("SELECT id_answer FROM answers WHERE id_question = :id AND correct = 1 AND deleted = 0");
        qq.bindValue(":id", q.value(0).toInt());
        if(!qq.exec())
        {
            ui->tableWidget->clear();
            ui->tableWidget->setRowCount(0);
            ui->tableWidget->setColumnCount(0);
            throwDBError(qq.lastError());
            return;
        }

        int qqSize = MainWindow::qSize(qq);

        if(qqSize == 0) //Якщо немає правильних відповідей
        {
            itm1->setBackgroundColor(QColor(240, 92, 92));
            itm2->setBackgroundColor(QColor(240, 92, 92));
        }
        else if(qqSize == count) //Якщо кількість правильних відповідей == загальній кількості відповідей
        {
            itm1->setBackgroundColor(QColor(242, 215, 62));
            itm2->setBackgroundColor(QColor(242, 215, 62));
        }
        else //Якщо все добре
        {
            itm1->setBackgroundColor(QColor(Qt::white));
            itm2->setBackgroundColor(QColor(Qt::white));
        }

        ui->tableWidget->setRowHeight(i, 40);
        i++;
    }
}

bool Questionary::askForChoice(int problemQuestionNumber, QString message)
{
    int i = problemQuestionNumber;

    if(QMessageBox::critical(this, tr("Проблема із створенням"), tr("Питання під номером ") + QString::number(i+1) + " " + message, tr("Так"), tr("Ні")) == 0)
    {
        AddQuestion *wnd = new AddQuestion(this);
        connect(this, SIGNAL(sendDataToQuestionEdit(bool,int,QString,int)), wnd, SLOT(recievedData(bool,int,QString,int)));
        connect(wnd, SIGNAL(sendDataToQuestionary(QString,int,int,int)), this, SLOT(recieveDataFromQuestions(QString,int,int,int)));
        emit(sendDataToQuestionEdit(true, ui->tableWidget->item(i,0)->data(Qt::UserRole).toInt(),
                                ui->tableWidget->item(i, 0)->text(), i));
        wnd->show();

        if(!wnd->exec())
            delete wnd;

        return true;
    }
    return false;
}

void Questionary::fillNameLine(QString themeName)
{
    QSqlQuery q;
    q.prepare("SELECT count(id_questionary) FROM questionary WHERE name_questionary LIKE :name");
    q.bindValue(":name", "Тест до теми «" + themeName + "»" + '%');
    if(!q.exec())
    {
        throwDBError(q.lastError());
        return;
    }
    q.first();
    int count = q.value(0).toInt();
    if(count > 0)
        ui->nameLine->setText(tr("Тест до теми «") + themeName + "» (" + QString::number(count + 1) + ")");
    else
        ui->nameLine->setText(tr("Тест до теми «") + themeName + "»");
}

void Questionary::throwDBError(QSqlError error)
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


void Questionary::on_addButton_clicked()
{
    ui->tableWidget->setFocus();

    //Попередні провірки
    if(ui->nameLine->text().isEmpty())
    {
        QMessageBox::information(this,tr("Підказка"),tr("Введіть назву тестувальника"), tr("Гаразд"));
        return;
    }

    int selectedRows = 0;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        if(ui->tableWidget->item(i,0)->isSelected())
            selectedRows++;

    if(selectedRows < 3)
    {
        QMessageBox::critical(this, tr("Проблема із створенням"), tr("Оберіть хоча б 3 питання"), tr("Гаразд"));
        return;
    }

    //Перевірка на наявність правильної відповіді
    while(true)
    {
        bool flag = true;
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            if((ui->tableWidget->item(i,0)->isSelected()) && (ui->tableWidget->item(i,0)->backgroundColor() == QColor(240, 92, 92)))
            {
                bool temp = askForChoice(i, tr("не має жодної правильної відповіді. \nБажаєте перейти до його редагування?"));
                if(!temp)
                    return;
                flag = false;
                break;
            }

            if((ui->tableWidget->item(i,0)->isSelected()) && (ui->tableWidget->item(i,0)->backgroundColor() == QColor(242, 215, 62)))
            {
                bool temp = askForChoice(i, tr("має лише правильні відповіді. \nБажаєте перейти до його редагування?"));
                if(!temp)
                    return;
                flag = false;
                break;
            }
        }
        if(flag)
            break;
    }

    //Перевірка на збіг у назві
    QSqlQuery q;
    q.prepare("select id_questionary from questionary where name_questionary = :name");
    q.bindValue(":name", ui->nameLine->text());
    if(q.exec())
    {
        if(MainWindow::qSize(q) > 0)
        {
            QMessageBox::critical(this,tr("Збіг даних"),tr("Такий тестувальник вже існує. Введіть іншу назву"), tr("Гаразд"));
            return;
        }
    }
    else
    {
        throwDBError(q.lastError());
        return;
    }

    QVector<QString> vecStr;
    QVector<QString> vecInt;
    QVector<int>     id;
    int qCount = 0;
    int id_questionary = 0;

    //Додавання в таблицю questionary
    q.prepare(QString("insert into questionary (name_questionary,createdBy,date_created,id_theme) values (:name, :user, %1, :theme)").
              arg((SelectDatabase::getDBLocation() == true)?"datetime('now','localtime')":"now()"));
    q.bindValue(":name", ui->nameLine->text());
    q.bindValue(":user", Login::getUsername());
    q.bindValue(":theme", id_theme);
    if(q.exec())
    {
        id_questionary = 0;

        if(!q.exec("select max(id_questionary) from questionary"))
        {
            throwDBError(q.lastError());
            return;
        }
        q.first();
        id_questionary = q.value(0).toInt();
        qDebug() << "id_questionary" << id_questionary;

        //Додавання в таблицю questions_questionary
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
            if(ui->tableWidget->item(i,0)->isSelected())
            {
                q.prepare("insert into questions_questionary (id_questionary,id_question) values (:id_1, :id_2)");
                q.bindValue(":id_1", id_questionary);
                q.bindValue(":id_2", ui->tableWidget->item(i,0)->data(Qt::UserRole).toInt());
                if(!q.exec())
                {
                    throwDBError(q.lastError());
                    return;
                }
                vecStr.append(ui->tableWidget->item(i,0)->text());
                vecInt.append(ui->tableWidget->item(i,1)->text());
                id.append(ui->tableWidget->item(i,0)->data(Qt::UserRole).toInt());
                qCount++;
            }
    }
    else
    {
        throwDBError(q.lastError());
        return;
    }

    int dlgResult = QMessageBox::question(this, tr("Операція успішна"), tr("Тестувальник успішно створений. \nОберіть подальшу дію:"), tr("Детальніше"), tr("Перейти до друку"), tr("Закрити"));

    if(dlgResult == 2)
        return;

    if(dlgResult == 0)
    {
        QuestionaryInfo *wnd = new QuestionaryInfo();
        connect(this, SIGNAL(sendDataToQuestionaryInfo(QString,QString,QString,QVector<QString>,QVector<QString>,QVector<int>)),
                wnd, SLOT(recieveDataFromQuestionary(QString,QString,QString,QVector<QString>,QVector<QString>,QVector<int>)));
        emit sendDataToQuestionaryInfo(ui->nameLine->text(), ui->lblTheme->text(), QString::number(qCount), vecStr, vecInt, id);
        wnd->show();
        if(!wnd->exec())
        {
            delete wnd;
            ui->tableWidget->setFocus();
        }
    }

    if(dlgResult == 1)
    {

        PrintQuestionary *wnd = new PrintQuestionary(this);
        connect(this, SIGNAL(sendDataToPrint(int,QString,QString,QString)), wnd, SLOT(recieveDataFromQuestionaryList(int,QString,QString,QString)));
        emit sendDataToPrint(id_questionary,
                             ui->nameLine->text(),
                             ui->lblSubject->text(),
                             ui->lblTheme->text());
        wnd->show();
        if(!wnd->exec())
            delete wnd;
    }
}

void Questionary::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    AddQuestion *wnd = new AddQuestion(this);
    connect(this, SIGNAL(sendDataToQuestionEdit(bool,int,QString,int)), wnd, SLOT(recievedData(bool,int,QString,int)));
    connect(wnd, SIGNAL(sendDataToQuestionary(QString,int,int,int)), this, SLOT(recieveDataFromQuestions(QString,int,int,int)));
    emit(sendDataToQuestionEdit(true, ui->tableWidget->currentItem()->data(Qt::UserRole).toInt(),
                            ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->text(),
                            ui->tableWidget->currentRow()));
    wnd->show();

    if(!wnd->exec())
        delete wnd;



}

void Questionary::recieveDataFromQuestions(QString qText, int aCount, int correctAnswersCount, int row)
{
    ui->tableWidget->item(row, 0)->setText(qText);
    ui->tableWidget->item(row, 1)->setText(QString::number(aCount));

    if(aCount == 0)
        ui->tableWidget->item(row, 1)->setFont(QFont("Segoe UI", 9, QFont::Bold));
    else
        ui->tableWidget->item(row, 1)->setFont(QFont("Segoe UI", 9, QFont::Normal));

    if(correctAnswersCount == 0)
    {
        ui->tableWidget->item(row, 0)->setBackgroundColor(QColor(240, 92, 92));
        ui->tableWidget->item(row, 1)->setBackgroundColor(QColor(240, 92, 92));
    }
    else if(aCount == correctAnswersCount)
    {
        ui->tableWidget->item(row, 0)->setBackgroundColor(QColor(242, 215, 62));
        ui->tableWidget->item(row, 1)->setBackgroundColor(QColor(242, 215, 62));
    }
    else
    {
        ui->tableWidget->item(row, 0)->setBackgroundColor(QColor(Qt::white));
        ui->tableWidget->item(row, 1)->setBackgroundColor(QColor(Qt::white));
    }
}

void Questionary::recieveDataFromThemes(int id_thme, QString name_subject, QString name_section, QString name_theme)
{
    id_theme = id_thme;
    if(id_theme == -1)
    {
        ui->addButton->setEnabled(false);
        ui->editButton->setEnabled(false);
        ui->lblSubject->setStyleSheet("margin-bottom: 6; color: red;");
        ui->lblSection->setStyleSheet("margin-bottom: 6; color: red;");
        ui->lblTheme->setStyleSheet("margin-bottom: 6; color: red;");
        this->setWindowTitle(tr("Створення тестувальника"));
        ui->nameLine->setText(tr("Оберіть тему"));
        ui->nameLine->setStyleSheet("color: red;");
        ui->nameLine->setReadOnly(true);
    }
    else
    {
        ui->addButton->setEnabled(true);
        ui->editButton->setEnabled(true);
        ui->lblSubject->setStyleSheet("margin-bottom: 6;");
        ui->lblSection->setStyleSheet("margin-bottom: 6;");
        ui->lblTheme->setStyleSheet("margin-bottom: 6;");
        this->setWindowTitle(tr("Створення тестувальника до теми \"") + name_theme + "\"");
        fillNameLine(name_theme);
        ui->nameLine->setReadOnly(false);
        ui->nameLine->setStyleSheet("");
    }
    ui->lblSubject->setText(name_subject);
    ui->lblSubject->setToolTip(name_subject);
    ui->lblSection->setText(name_section);
    ui->lblSection->setToolTip(name_section);
    ui->lblTheme->setText(name_theme);
    ui->lblTheme->setToolTip(name_theme);
    updateTable();
}

void Questionary::recieveDataFromQuestionaryList(int id_thme, QString name_subject, QString name_section, QString name_theme, bool themeBtn)
{
    ui->themeButton->setEnabled(themeBtn);
    setModal(!themeBtn);
    id_theme = id_thme;

    ui->addButton->setEnabled(true);
    ui->editButton->setEnabled(true);
    ui->lblSubject->setStyleSheet("margin-bottom: 6;");
    ui->lblSection->setStyleSheet("margin-bottom: 6;");
    ui->lblTheme->setStyleSheet("margin-bottom: 6;");
    this->setWindowTitle(tr("Створення тестувальника до теми \"") + name_theme + "\"");
    ui->lblSubject->setText(name_subject);
    ui->lblSubject->setToolTip(name_subject);
    ui->lblSection->setText(name_section);
    ui->lblSection->setToolTip(name_section);
    ui->lblTheme->setText(name_theme);
    ui->lblTheme->setToolTip(name_theme);
    fillNameLine(name_theme);
    updateTable();
}

void Questionary::on_themeButton_clicked()
{
    HelpSubjects *wnd = new HelpSubjects();
    connect(this, SIGNAL(sendDataToThemes(bool)), wnd, SLOT(recieveData(bool)));
    connect(wnd, SIGNAL(sendData(int,QString,QString,QString,bool)),this,SLOT(recieveDataFromThemes(int,QString,QString,QString)));
    emit sendDataToThemes(true);
    wnd->show();
    if(!wnd->exec())
        delete wnd;
}

void Questionary::on_editButton_clicked()
{
    Questions *wnd = new Questions();
    connect(this, SIGNAL(sendDataToQuestions(int, QString, QString, QString, bool)), wnd, SLOT(recieveDataFromThemes(int, QString, QString, QString, bool)));
    emit sendDataToQuestions(id_theme,
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

void Questionary::on_randomButton_clicked()
{
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        ui->tableWidget->item(i,0)->setSelected(false);
        ui->tableWidget->item(i,1)->setSelected(false);
    }

    if(ui->spinBox->value() == ui->tableWidget->rowCount())
        ui->tableWidget->selectAll();
    else
    {
        int i = 0;
        int rnd = 0;
        while(i < ui->spinBox->value())
        {
            rnd = qrand() % ui->tableWidget->rowCount();
            if(!ui->tableWidget->item(rnd,0)->isSelected())
            {
                ui->tableWidget->selectRow(rnd);
                i++;
            }
        }
    }
    ui->tableWidget->setFocus();
}

void Questionary::checkLabelGrammar()
{
    int val = ui->spinBox->value();

    if(val == 100)
    {
        ui->valueLabel->setText(tr("випадкових питань"));
        return;
    }

    if((val >= 5) && (val <= 20))
    {
        ui->valueLabel->setText(tr("випадкових питань"));
        return;
    }

    if(val == 1)
    {
        ui->valueLabel->setText(tr("випадкове питання"));
        return;
    }

    if((val >= 2) && (val <= 4))
    {
        ui->valueLabel->setText(tr("випадкових питання"));
        return;
    }

    int temp = val % 10;

    if(temp == 0)
    {
        ui->valueLabel->setText(tr("випадкових питань"));
        return;
    }

    if(temp == 1)
    {
        ui->valueLabel->setText(tr("випадкове питання"));
        return;
    }

    if((temp >= 2) && (temp <= 4))
    {
        ui->valueLabel->setText(tr("випадкових питання"));
        return;
    }

    if((temp >= 5) && (temp <= 9))
    {
        ui->valueLabel->setText(tr("випадкових питань"));
        return;
    }
}

