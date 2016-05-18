#include "users.h"
#include "ui_users.h"

Users::Users(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Users)
{
    ui->setupUi(this);
    isDeleted = false;
    updateTable();

    this->setWhatsThis(tr("Форма, доступ до якої має лише адміністратор. Дозволяє реєструвати нових користувачів, редагувати чи видаляти існуючих."));
    ui->addButton->setWhatsThis(tr("Натисніть для реєстрації нового користувача."));
    ui->editButton->setWhatsThis(tr("Натисніть для редагування обраного користувача."));
    ui->deleteButton->setWhatsThis(tr("Натисніть для видалення обраного користувача."));
    ui->tableWidget->setWhatsThis(tr("Інтерактивний список користувачів."));
    ui->closeButton->setWhatsThis(tr("Натисніть для закриття форми."));
}

Users::~Users()
{
    delete ui;
}

void Users::updateTable()
{
    ui->tableWidget->clear();
    QSqlQuery q;
    int i = 0;
    q.prepare("SELECT id, login, md5, username, email, createat, lastLogin, deleted FROM users");
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
            QTableWidgetItem *itm;
            if((k == 5) || (k == 6)) //if date
            {
                temp = q.value(k).toDateTime().toString();
                align = Qt::AlignCenter;
            }
            else if(k == 7) //if deleted
            {
                temp = (q.value(k).toInt()==1)?tr("Так"):tr("Ні");
                align = Qt::AlignCenter;
            }
            else
            {
                temp = q.value(k).toString();
                align = Qt::AlignVCenter;
            }

            itm = new QTableWidgetItem(temp);
            itm->setData(Qt::UserRole, q.value(0));
            itm->setTextAlignment(align);
            ui->tableWidget->setItem(i,k-1,itm);
        }
        ui->tableWidget->setRowHeight(i, 40);
        i++;
    }

    ui->tableWidget->setCurrentCell(0,0);
}

void Users::throwDBError(QSqlError error)
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

void Users::setColumns()
{
    ui->tableWidget->setColumnCount(7);
    QStringList list;
    list << tr("Логін") << tr("md5-сума") << tr("Ім'я") << tr("E-mail") << tr("Дата створення") << tr("Востаннє заходив") << tr("Чи видалений");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->setColumnWidth(0, 150);
    ui->tableWidget->setColumnWidth(1, 220);
    ui->tableWidget->setColumnWidth(2, 150);
    ui->tableWidget->setColumnWidth(3, 150);
    ui->tableWidget->setColumnWidth(4, 125);
    ui->tableWidget->setColumnWidth(5, 125);
    ui->tableWidget->setColumnWidth(6, 100);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
}

void Users::on_closeButton_clicked()
{
    this->close();
}

void Users::on_deleteButton_clicked()
{
    QSqlQuery query;
    query.prepare("update users set deleted = :d where id = :id");
    query.bindValue(":d", (isDeleted == true)?0:1);
    query.bindValue(":id", ui->tableWidget->currentItem()->data(Qt::UserRole).toInt());
    if(!query.exec())
    {
        throwDBError(query.lastError());
        return;
    }
    ui->tableWidget->item(ui->tableWidget->currentRow(), 6)->setText((isDeleted == true)?tr("Ні"):tr("Так"));
    ui->lblInfo->setText((isDeleted == true)?tr("Користувач успішно відновлений"):tr("Користувач успішно видалений"));
    emit on_tableWidget_itemSelectionChanged();
    ui->tableWidget->setFocus();
}

void Users::on_addButton_clicked()
{
    int row = ui->tableWidget->currentRow();
    int rowCount = ui->tableWidget->rowCount();
    Register *w = new Register();
    w->show();
    if(!w->exec())
    {
        delete w;
        updateTable();
    }
    if(ui->tableWidget->rowCount() > rowCount)
    {
        ui->lblInfo->setText(tr("Користувач успішно доданий"));
        ui->tableWidget->setCurrentCell(ui->tableWidget->rowCount()-1,0);
    }
    else
    {
        ui->lblInfo->setText(tr("Ви не додали користувача"));
        ui->tableWidget->setCurrentCell(row,0);
    }
    ui->tableWidget->setFocus();
}

void Users::on_editButton_clicked()
{
    int row = ui->tableWidget->currentRow();
    QStringList list;
    list << ui->tableWidget->item(row,0)->text() << ui->tableWidget->item(row,1)->text() << ui->tableWidget->item(row,2)->text() << ui->tableWidget->item(row,3)->text();
    Register *w = new Register();
    connect(this, SIGNAL(sendDataToRegister(int,QString,QString,QString,QString,QString)), w, SLOT(recieveData(int,QString,QString,QString,QString,QString)));
    emit sendDataToRegister(ui->tableWidget->currentItem()->data(Qt::UserRole).toInt(),
                            list[0],
                            list[2],
                            list[3],
                            tr("Редагування користувача"),
                            tr("Редаг&увати"));
    w->show();
    if(!w->exec())
    {
        delete w;
        updateTable();
        ui->tableWidget->setCurrentCell(row,0);
    }

    bool isEdited = false;
    for(int i = 0; i < 4; i++)
        if(ui->tableWidget->item(row,i)->text() != list[i])
            isEdited = true;

    ui->lblInfo->setText((isEdited == true)?tr("Успішно відредаговоно"):tr("Редагування не відбулося"));
}

void Users::on_tableWidget_itemSelectionChanged()
{
    if(ui->tableWidget->currentItem()->isSelected())
    {
        ui->editButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        isDeleted = (ui->tableWidget->item(ui->tableWidget->currentRow(),6)->text() == tr("Так"))?true:false;
        if(isDeleted)
            ui->deleteButton->setText(tr("Відновити"));
        else
            ui->deleteButton->setText(tr("Видалити"));
    }
    else
    {
        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    }
}

void Users::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    emit on_editButton_clicked();
}
