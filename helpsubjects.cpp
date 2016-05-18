#include "helpsubjects.h"
#include "ui_helpsubjects.h"

HelpSubjects::HelpSubjects(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpSubjects)
{
    ui->setupUi(this);

    connect(ui->textFilter, SIGNAL(textChanged()), this, SLOT(filterSlot()));

    this->setWhatsThis(tr("Ця форма являє собою своєрідний довідник, який надає змоги перегляду, додавання, редагування та видалення \
                                предметів/розділів/тем у базі даних."));
    ui->addButton->setWhatsThis(tr("Натисніть для додавання нового пункта (предмета/розділа/теми) у базу даних. \n\nЗа певних умов, кнопка може перебувати у неактивному стані, адже існує система перевірок, яка запобігає додаванню нового розділа, якщо у базі не міститься жодного предмета. Також ця система запобігає додаванню нової теми, якщо не обраний жоден розділ."));
    ui->editButton->setWhatsThis(tr("Натисність для редагування пункта (предмета/розділа/теми). \n\nЗа певних умов, кнопка може перебувати у неактивному стані, адже існує система перевірок, яка запобігає редагуванню пункта, якщо він не обраний зі списка."));
    ui->deleteButton->setWhatsThis(tr("Натисніть для видалення пункта (предмета/розділа/теми) із бази даних. \n\nЗа певних умов, кнопка може перебувати у неактивному стані, адже існує система перевірок, яка запобігає видаленню пункта, якщо він не обраний зі списка."));
    ui->clearFiltersButton->setWhatsThis(tr("Натисніть для очищення усіх полей фільтрації."));
    ui->closeButton->setWhatsThis(tr("Кнопка має декілька режимів: закриття, відміни та вибору теми. \nЯкщо активована функція множинного редагування,\
                                то, при натисненні на кнопку, множинне видалення відміняється. \nЯкщо поточне вікно перебуває у стані вибору теми, \
                                то, при натисненні на кпопку, ви обираєте поточну тему. \nВ іншому ж випадку, кнопка просто закриває поточну форму."));

    ui->multiDeleteButton->setWhatsThis(tr("Натисніть для початку множинного видалення, яке дозволяє видаляти декільки пунктів \
                                (предметів/розділів/тем) одночасно."));
    ui->subjectsList->setWhatsThis(tr("Інтерактивний список предметів, які містяться у базі даних."));
    ui->sectionsList->setWhatsThis(tr("Інтерактивний список розділів, які містяться у базі даних."));
    ui->themesList->setWhatsThis(tr("Інтерактивний список тем, які містяться у базі даних."));
    ui->groupBox_4->setWhatsThis(tr("Область форми, яка відповідає за операції додавання, редагування та видалення предметів/розділів/тем."));
    ui->rbActionsSubjects->setWhatsThis(tr("Оберіть для виконання операцій над предметами."));
    ui->rbActionsSections->setWhatsThis(tr("Оберіть для виконання операцій над розділами."));
    ui->rbActionsThemes->setWhatsThis(tr("Оберіть для виконання операцій над темами."));
    ui->groupBox_5->setWhatsThis(tr("Область форми, яка відповідає за фільтрацію списків та пошук інформації у базі даних."));
    ui->rbFilterSubjects->setWhatsThis(tr("Оберіть для фільтрування списку предметів."));
    ui->rbFilterSections->setWhatsThis(tr("Оберіть для фільтрування списку розділів."));
    ui->rbFilterThemes->setWhatsThis(tr("Оберіть для фільтрування списку тем."));
    ui->textFilter->setWhatsThis(tr("Текстове поле, в якому задається пошукове слово для категорії, обраної зліва."));

    multipleDelete = false;
    behavior = false;
    closeBehavior = false;
    for(int i = 0; i < 3; i++)
        filters[i] = "";
    ui->rbActionsSubjects->setChecked(true);
    ui->rbFilterSubjects->setChecked(true);

    QSqlQuery q;
    if(!q.exec("SELECT id_theme FROM themes LIMIT 1"))
    {
        throwDBError(q.lastError());
        return;
    }

    if(MainWindow::checkRecordsCount("id_subject", "subjects"))
    {
        updateLists();
        updateSubjectsList();
    }
    emit on_rbActionsSubjects_clicked();
}

HelpSubjects::~HelpSubjects()
{
    delete ui;
}

void HelpSubjects::updateLists()
{
    mainSubjectsList.clear();
    mainSectionsList.clear();
    mainThemesList.clear();

    QSqlQuery q;
    q.prepare("SELECT id_subject, name_subject FROM subjects WHERE deleted = 0");
    if(q.exec())
        while(q.next())
        {
            QListWidgetItem *itm = new QListWidgetItem(q.value(1).toString());
            itm->setData(Qt::UserRole, q.value(0).toInt());
            mainSubjectsList.append(itm);
        }
    else
        throwDBError(q.lastError());

    q.prepare("SELECT id_section, id_subject, name_section FROM sections WHERE deleted = 0");
    if(q.exec())
        while(q.next())
        {
            QListWidgetItem *itm = new QListWidgetItem(q.value(2).toString());
            itm->setData(Qt::UserRole, QPoint(q.value(0).toInt(), q.value(1).toInt()));
            mainSectionsList.append(itm);
        }
    else
        throwDBError(q.lastError());

    q.prepare("SELECT id_theme, id_section, name_theme FROM themes WHERE deleted = 0");
    if(q.exec())
        while(q.next())
        {
            QListWidgetItem *itm = new QListWidgetItem(q.value(2).toString());
            itm->setData(Qt::UserRole, QPoint(q.value(0).toInt(), q.value(1).toInt()));
            mainThemesList.append(itm);
        }
    else
        throwDBError(q.lastError());
}

void HelpSubjects::recieveData(bool flag)
{
    behavior = flag;
    ui->closeButton->setText(tr("Обрати тему"));
    this->setModal(true);
}

void HelpSubjects::closeEvent(QCloseEvent *e)
{
    if(behavior && !closeBehavior)
        emit sendData(-1, "Оберіть тему", "Оберіть тему", "Оберіть тему", true);
}

void HelpSubjects::updateSubjectsList()
{
    ui->subjectsList->clear();

    foreach (QListWidgetItem *itm, mainSubjectsList)
    {
        QListWidgetItem *i = new QListWidgetItem(*itm);
        ui->subjectsList->addItem(i);
    }
    ui->subjectsList->setCurrentRow(0);
}

void HelpSubjects::updateSectionsList()
{
    ui->sectionsList->clear();

    foreach (QListWidgetItem *itm, mainSectionsList)
    {
        if(itm->data(Qt::UserRole).toPoint().y() == ui->subjectsList->currentItem()->data(Qt::UserRole).toInt())
        {
            QListWidgetItem *i = new QListWidgetItem(*itm);
            ui->sectionsList->addItem(i);
        }
    }
    ui->sectionsList->setCurrentRow(0);
}

void HelpSubjects::updateThemesList()
{
    ui->themesList->clear();

    foreach (QListWidgetItem *itm, mainThemesList)
    {
        if(itm->data(Qt::UserRole).toPoint().y() == ui->sectionsList->currentItem()->data(Qt::UserRole).toPoint().x())
        {
            QListWidgetItem *i = new QListWidgetItem(*itm);
            ui->themesList->addItem(i);
        }
    }
    ui->themesList->setCurrentRow(0);
}

void HelpSubjects::on_subjectsList_itemSelectionChanged()
{
    if(multipleDelete)
        return;

    if(canUpdateSectionsList())
        updateSectionsList();
    else
    {
        ui->sectionsList->clear();
        ui->themesList->clear();
    }

    if(ui->rbActionsSubjects->isChecked()) emit on_rbActionsSubjects_clicked();
    if(ui->rbActionsSections->isChecked()) emit on_rbActionsSections_clicked();
    if(ui->rbActionsThemes->isChecked()) emit on_rbActionsThemes_clicked();
}

void HelpSubjects::on_sectionsList_itemSelectionChanged()
{
    if(multipleDelete)
        return;

    if(canUpdateThemesList())
        updateThemesList();
    else
        ui->themesList->clear();

    if(ui->rbActionsSubjects->isChecked()) emit on_rbActionsSubjects_clicked();
    if(ui->rbActionsSections->isChecked()) emit on_rbActionsSections_clicked();
    if(ui->rbActionsThemes->isChecked()) emit on_rbActionsThemes_clicked();
}

void HelpSubjects::on_themesList_itemSelectionChanged()
{
    if(multipleDelete)
        return;

    if(ui->rbActionsSubjects->isChecked()) emit on_rbActionsSubjects_clicked();
    if(ui->rbActionsSections->isChecked()) emit on_rbActionsSections_clicked();
    if(ui->rbActionsThemes->isChecked()) emit on_rbActionsThemes_clicked();
}

bool HelpSubjects::canUpdateSectionsList()
{
    foreach (QListWidgetItem *i, mainSectionsList)
    {
        if(i->data(Qt::UserRole).toPoint().y() == ui->subjectsList->currentItem()->data(Qt::UserRole).toInt())
            return true;
    }
    return false;
}

bool HelpSubjects::canUpdateThemesList()
{
    foreach (QListWidgetItem *i, mainThemesList)
    {
        if(i->data(Qt::UserRole).toPoint().y() == ui->sectionsList->currentItem()->data(Qt::UserRole).toPoint().x())
            return true;
    }
    return false;
}

QString HelpSubjects::inputData(QString title, QString label, QString defaultTextValue)
{
    QInputDialog *dlg = new QInputDialog(this);
    dlg->setInputMode(QInputDialog::TextInput);
    dlg->setCancelButtonText(tr("Відмінити"));
    if(defaultTextValue.isEmpty())
        dlg->setOkButtonText(tr("Додати"));
    else
        dlg->setOkButtonText(tr("Редагувати"));
    dlg->setWindowTitle(title);
    dlg->setLabelText(label);
    dlg->setTextValue(defaultTextValue);
    bool ok = dlg->exec();
    QString text = dlg->textValue();
    delete dlg;
    if(ok == QDialog::Rejected)
        return "";
    else
        return text;
}

void HelpSubjects::on_addButton_clicked()
{
    if(ui->rbActionsSubjects->isChecked())
    {
        QString text = inputData(tr("Новий предмет"), tr("Введіть назву предмета:"), "");

        if(!text.isEmpty())
        {
            if (!MainWindow::checkMatch(text,"subjects","name_subject"))
            {
                QMessageBox::critical(this,tr("Збіг даних"), tr("Такий предмет вже існує"), tr("Гаразд"));
                return;
            }

            QSqlQuery q;
            q.prepare("INSERT INTO subjects (name_subject) VALUES (:name)");
            q.bindValue(":name", text);
            if(q.exec())
            {
                updateLists();
                updateSubjectsList();
                ui->subjectsList->setCurrentRow(ui->subjectsList->count()-1);
            }
            else
            {
                throwDBError(q.lastError());
                return;
            }

            QMessageBox::information(this, tr("Успіх"), tr("Новий предмет успішно додано"), tr("Гаразд"));
            emit on_rbActionsSubjects_clicked();
            return;
        }
    }

    if(ui->rbActionsSections->isChecked())
    {
        int curSubjectIndex = ui->subjectsList->currentItem()->data(Qt::UserRole).toInt();
        QString text = inputData(tr("Нового розділ"), tr("Введіть назву розділа:"), "");
        if(!text.isEmpty())
        {
            if (!MainWindow::checkMatch(text,"sections","name_section","id_subject",curSubjectIndex))
            {
                qDebug() << "Є збіг";
                QMessageBox::critical(this,tr("Збіг даних"), tr("Такий розділ вже існує"), tr("Гаразд"));
                return;
            }

            QSqlQuery q;
            q.prepare("INSERT INTO sections (id_subject,name_section) VALUES (:id, :name)");
            q.bindValue(":id", curSubjectIndex);
            q.bindValue(":name", text);
            if(q.exec())
            {
                updateLists();
                updateSectionsList();
                ui->sectionsList->setCurrentRow(ui->sectionsList->count()-1);
            }
            else
            {
                throwDBError(q.lastError());
                return;
            }

            QMessageBox::information(this, tr("Успіх"), tr("Новий розділ успішно додано"), tr("Гаразд"));
            emit on_rbActionsSections_clicked();
            return;
        }
    }

    if(ui->rbActionsThemes->isChecked())
    {
        int curSectionIndex = ui->sectionsList->currentItem()->data(Qt::UserRole).toPoint().x();
        QString text = inputData(tr("Нової тема"), tr("Введіть назву теми:"), "");
        if(!text.isEmpty())
        {
            if (!MainWindow::checkMatch(text,"themes","name_theme","id_section",curSectionIndex))
            {
                qDebug() << "Є збіг";
                QMessageBox::critical(this,tr("Збіг даних"), tr("Така тема вже існує"), tr("Гаразд"));
                return;
            }

            QSqlQuery q;
            q.prepare("INSERT INTO themes (id_section, name_theme) VALUES (:id, :name)");
            q.bindValue(":id", curSectionIndex);
            q.bindValue(":name", text);
            if(q.exec())
            {
                updateLists();
                updateThemesList();
                ui->themesList->setCurrentRow(ui->themesList->count()-1);
            }
            else
            {
                throwDBError(q.lastError());
                return;
            }
            QMessageBox::information(this, tr("Успіх"), tr("Нова тема успішно додана"), tr("Гаразд"));
            emit on_rbActionsThemes_clicked();
            return;
        }
    }
}

void HelpSubjects::on_editButton_clicked()
{
    if(ui->rbActionsSubjects->isChecked())
    {
        QString text = inputData(tr("Редагування"), tr("Введіть назву предмета"), ui->subjectsList->currentItem()->text());
        if(!text.isEmpty())
        {
            if (!MainWindow::checkMatch(text,"subjects","name_subject"))
            {
                QMessageBox::critical(this,tr("Збіг даних"), tr("Такий предмет вже існує"), tr("Гаразд"));
                return;
            }

            int currRow = ui->subjectsList->currentRow();
            QSqlQuery q;
            q.prepare("UPDATE subjects SET name_subject = :name WHERE id_subject = :id");
            q.bindValue(":name", text);
            q.bindValue(":id", ui->subjectsList->currentItem()->data(Qt::UserRole).toInt());
            if(q.exec())
            {
                updateLists();
                updateSubjectsList();
                ui->subjectsList->setCurrentRow(currRow);
                QMessageBox::information(this, tr("Успіх"), tr("Предмет успішно відредагований"), tr("Гаразд"));
            }
            else
            {
                throwDBError(q.lastError());
                return;
            }

        }

        emit on_rbActionsSubjects_clicked();
        return;
    }

    if(ui->rbActionsSections->isChecked())
    {
        QString text = inputData(tr("Редагування"),tr("Введіть назву розділа"),ui->sectionsList->currentItem()->text());
        if(!text.isEmpty())
        {
            if (!MainWindow::checkMatch(text,"sections","name_section"))
            {
                qDebug() << "Є збіг";
                QMessageBox::critical(this,tr("Збіг даних"), tr("Такий розділ вже існує"), tr("Гаразд"));
                return;
            }

            int currRow = ui->sectionsList->currentRow();
            QSqlQuery q;
            q.prepare("UPDATE sections SET name_section = :name WHERE id_section = :id");
            q.bindValue(":name", text);
            q.bindValue(":id", ui->sectionsList->currentItem()->data(Qt::UserRole).toPoint().x());
            if(q.exec())
            {
                updateLists();
                updateSectionsList();
                ui->sectionsList->setCurrentRow(currRow);
                QMessageBox::information(this, tr("Успіх"), tr("Розділ успішно відредагований"), tr("Гаразд"));
            }
            else
            {
                throwDBError(q.lastError());
                return;
            }

        }

        emit on_rbActionsSections_clicked();
        return;
    }

    if(ui->rbActionsThemes->isChecked())
    {
        QString text = inputData(tr("Редагування"),tr("Введіть назву теми"),ui->themesList->currentItem()->text());
        if(!text.isEmpty())
        {
            if (!MainWindow::checkMatch(text,"themes","name_theme"))
            {
                qDebug() << "Є збіг";
                QMessageBox::critical(this,tr("Збіг даних"), tr("Така тема вже існує"), tr("Гаразд"));
                return;
            }

            int currRow = ui->themesList->currentRow();
            QSqlQuery q;
            q.prepare("UPDATE themes SET name_theme = :name WHERE id_theme = :id");
            q.bindValue(":name", text);
            q.bindValue(":id", ui->themesList->currentItem()->data(Qt::UserRole).toPoint().x());
            if(q.exec())
            {
                updateLists();
                updateThemesList();
                ui->themesList->setCurrentRow(currRow);
                QMessageBox::information(this, tr("Успіх"), tr("Предмет успішно відредагований"), tr("Гаразд"));
            }
            else
            {
                throwDBError(q.lastError());
                return;
            }

        }

        emit on_rbActionsThemes_clicked();
        return;
    }
}

void HelpSubjects::on_rbActionsSubjects_clicked()
{
    if(multipleDelete)
        return;

    ui->addButton->setEnabled(true);
    if(ui->subjectsList->count() > 0)
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

void HelpSubjects::on_rbActionsSections_clicked()
{
    if(multipleDelete)
        return;

    if(ui->subjectsList->count() > 0)
        ui->addButton->setEnabled(true);
    else
        ui->addButton->setEnabled(false);

    if(ui->sectionsList->count() > 0)
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

void HelpSubjects::on_rbActionsThemes_clicked()
{
    if(multipleDelete)
        return;

    if(ui->sectionsList->count() > 0)
        ui->addButton->setEnabled(true);
    else
        ui->addButton->setEnabled(false);

    if(ui->themesList->count() > 0)
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

void HelpSubjects::filterSlot()
{
    if(multipleDelete)
        return;

    if(ui->rbFilterSubjects->isChecked())
    {
        filters[0] = ui->textFilter->toPlainText();

        if(ui->textFilter->toPlainText().isEmpty())
            updateSubjectsList();
        else
        {
            ui->subjectsList->clear();
            filterSubjectsList.clear();

            for(int i = 0; i < mainSubjectsList.size(); i++)
            {
                if(mainSubjectsList.at(i)->text().contains(ui->textFilter->toPlainText(), Qt::CaseInsensitive))
                {
                    QListWidgetItem *tmp = new QListWidgetItem(*mainSubjectsList[i]);
                    filterSubjectsList.append(tmp);
                }
            }

            foreach (QListWidgetItem *itm, filterSubjectsList)
            {
                QListWidgetItem *i = new QListWidgetItem(*itm);
                ui->subjectsList->addItem(i);
            }

            filterSubjectsList.clear();
            ui->subjectsList->setCurrentRow(0);
        }

        if(ui->subjectsList->count() < 1)
        {
            ui->sectionsList->clear();
            ui->themesList->clear();
        }
        emit on_rbActionsSubjects_clicked();
    }

    if(ui->rbFilterSections->isChecked())
    {
        filters[1] = ui->textFilter->toPlainText();

        if(ui->subjectsList->count() < 1)
            return;

        if(ui->textFilter->toPlainText().isEmpty())
            updateSectionsList();
        else
        {
            ui->sectionsList->clear();
            filterSectionsList.clear();

            for(int i = 0; i < mainSectionsList.size(); i++)
            {
                if((mainSectionsList.at(i)->text().contains(ui->textFilter->toPlainText(), Qt::CaseInsensitive)) &&
                        (mainSectionsList.at(i)->data(Qt::UserRole).toPoint().y() == ui->subjectsList->currentItem()->data(Qt::UserRole).toInt()))
                {
                    QListWidgetItem *tmp = new QListWidgetItem(*mainSectionsList[i]);
                    filterSectionsList.append(tmp);
                }
            }

            foreach (QListWidgetItem *itm, filterSectionsList)
            {
                QListWidgetItem *i = new QListWidgetItem(*itm);
                ui->sectionsList->addItem(i);
            }

            filterSectionsList.clear();
            ui->sectionsList->setCurrentRow(0);
        }

        if(ui->sectionsList->count() < 1)
            ui->themesList->clear();

        emit on_rbActionsSections_clicked();
    }

    if(ui->rbFilterThemes->isChecked())
    {
        filters[2] = ui->textFilter->toPlainText();

        if(ui->sectionsList->count() < 1)
            return;

        if(ui->textFilter->toPlainText().isEmpty())
            updateThemesList();
        else
        {
            ui->themesList->clear();
            filterThemesList.clear();

            for(int i = 0; i < mainThemesList.size(); i++)
            {
                if((mainThemesList.at(i)->text().contains(ui->textFilter->toPlainText(), Qt::CaseInsensitive)) &&
                        (mainThemesList.at(i)->data(Qt::UserRole).toPoint().y() == ui->sectionsList->currentItem()->data(Qt::UserRole).toPoint().x()))
                {
                    QListWidgetItem *tmp = new QListWidgetItem(*mainThemesList[i]);
                    filterThemesList.append(tmp);
                }
            }

            foreach (QListWidgetItem *itm, filterThemesList)
            {
                QListWidgetItem *i = new QListWidgetItem(*itm);
                ui->themesList->addItem(i);
            }

            filterThemesList.clear();
            ui->themesList->setCurrentRow(0);
            emit on_rbActionsThemes_clicked();
        }
    }
}

void HelpSubjects::on_rbFilterSubjects_clicked()
{
    ui->textFilter->setText(filters[0]);
    ui->rbActionsSubjects->setChecked(true);
    emit on_rbActionsSubjects_clicked();
}

void HelpSubjects::on_rbFilterSections_clicked()
{
    ui->textFilter->setText(filters[1]);
    ui->rbActionsSections->setChecked(true);
    emit on_rbActionsSections_clicked();
}

void HelpSubjects::on_rbFilterThemes_clicked()
{
    ui->textFilter->setText(filters[2]);
    ui->rbActionsThemes->setChecked(true);
    emit on_rbActionsThemes_clicked();
}

void HelpSubjects::on_clearFiltersButton_clicked()
{
    ui->rbFilterSubjects->setChecked(true);
    ui->rbActionsSubjects->setChecked(true);
    for(int i = 0; i < 3; i++)
        filters[i] = "";
    ui->textFilter->setText("");
}

void HelpSubjects::on_closeButton_clicked()
{
    if(multipleDelete)
    {
        multipleDelete = !multipleDelete;
        ui->multiDeleteButton->setText("Множинне  \nвидалення");
        ui->closeButton->setText(behavior ? "Обрати тему" : "Закрити");

        ui->subjectsList->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->sectionsList->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->themesList->setSelectionMode(QAbstractItemView::SingleSelection);

        ui->subjectsList->setStyleSheet("");
        ui->sectionsList->setStyleSheet("");
        ui->themesList->setStyleSheet("");

        ui->subjectsList->setCurrentRow(0);
        ui->rbActionsSubjects->setChecked(true);
        emit on_rbActionsSubjects_clicked();
    }
    else
        if(behavior)
        {
            if(ui->themesList->currentRow() == -1)
            {

                if(QMessageBox::critical(this, tr("Оберіть тему"), tr("Ви не обрали тему. Бажаєте спробувати знову?"), tr("Так"), tr("Ні")) == 1)
                    close();
                return;
            }
            emit sendData(ui->themesList->currentItem()->data(Qt::UserRole).toPoint().x(),
                          ui->subjectsList->currentItem()->text(),
                          ui->sectionsList->currentItem()->text(),
                          ui->themesList->currentItem()->text(),
                          true);
            closeBehavior = true;
            close();
        }
        else
            close();
}

void HelpSubjects::on_deleteButton_clicked()
{
    if(ui->rbActionsSubjects->isChecked())
    {
        QString warning = "";
        warning = tr("Ви впевнені, що хочете видалити предмет \"") + ui->subjectsList->currentItem()->text() +
                tr("\"?\nВсі розділи та теми, пов'язані із цим предметом, також будуть видалені.\n");
        if(ui->sectionsList->count() > 0)
        {
            warning += tr("\nБудуть видалені наступні розділи:\n");
            for(int i = 0; i < ui->sectionsList->count(); i++)
            {
                warning += tr("\nРозділ: \"") + ui->sectionsList->item(i)->text() + "\"\n";
                warning += tr("Теми:\n");
                for(int j = 0; j < mainThemesList.size(); j++)
                    if(mainThemesList.at(j)->data(Qt::UserRole).toPoint().y() ==
                            ui->sectionsList->item(i)->data(Qt::UserRole).toPoint().x())
                        warning += "    \"" + mainThemesList.at(j)->text() + "\"\n";
            }
        }

        if(QMessageBox::question(this, tr("Підтвердіть Ваш вибір"), warning, tr("Так"), tr("Ні")) == 1)
            return;

        QSqlQuery q;

        for(int i = 0; i < ui->sectionsList->count(); i++)
        {
            q.prepare("UPDATE themes SET deleted = 1 WHERE id_section = :id");
            q.bindValue(":id", ui->sectionsList->item(i)->data(Qt::UserRole).toPoint().x());
            if(!q.exec())
            {
                throwDBError(q.lastError());
                return;
            }
        }

        q.prepare("UPDATE sections SET deleted = 1 WHERE id_subject = :id");
        q.bindValue(":id", ui->subjectsList->currentItem()->data(Qt::UserRole).toInt());
        if(!q.exec())
        {
            throwDBError(q.lastError());
            return;
        }

        q.prepare("UPDATE subjects SET deleted = 1 WHERE id_subject = :id");
        q.bindValue(":id", ui->subjectsList->currentItem()->data(Qt::UserRole).toInt());
        if(!q.exec())
        {
            throwDBError(q.lastError());
            return;
        }

        int curRow = ui->subjectsList->currentRow();
        updateLists();
        updateSubjectsList();
        if(curRow == 0)
            ui->subjectsList->setCurrentRow(0);
        else
            ui->subjectsList->setCurrentRow(curRow-1);
        QMessageBox::information(this, tr("Успіх"), tr("Обраний предмет був успішно видалений"), tr("Гаразд"));
        emit on_rbActionsSubjects_clicked();
        return;
    }

    if(ui->rbActionsSections->isChecked())
    {
        QString warning = "";
        warning = tr("Ви впевнені, що хочете видалити розділ \"") + ui->sectionsList->currentItem()->text() +
                tr("\"?\nВсі теми, пов'язані із цим розділом, також будуть видалені.\n");
        if(ui->themesList->count() > 0)
        {
            warning += tr("\nБудуть видалені наступні теми:\n");
            for(int i = 0; i < ui->themesList->count(); i++)
                warning += "    \"" + ui->themesList->item(i)->text() + "\"\n";
        }

        if(QMessageBox::question(this, tr("Підтвердіть Ваш вибір"), warning, tr("Так"), tr("Ні")) == 1)
            return;

        QSqlQuery q;

        q.prepare("UPDATE themes SET deleted = 1 WHERE id_section = :id");
        q.bindValue(":id", ui->sectionsList->currentItem()->data(Qt::UserRole).toPoint().x());
        if(!q.exec())
        {
            throwDBError(q.lastError());
            return;
        }

        q.prepare("UPDATE sections SET deleted = 1 WHERE id_section = :id");
        q.bindValue(":id", ui->sectionsList->currentItem()->data(Qt::UserRole).toPoint().x());
        if(!q.exec())
        {
            throwDBError(q.lastError());
            return;
        }

        int curRow = ui->sectionsList->currentRow();
        updateLists();
        updateSectionsList();
        if(curRow == 0)
            ui->sectionsList->setCurrentRow(0);
        else
            ui->sectionsList->setCurrentRow(curRow-1);
        QMessageBox::information(this, tr("Успіх"), tr("Обраний розділ був успішно видалений"), tr("Гаразд"));
        emit on_rbActionsSections_clicked();
        return;
    }

    if(ui->rbActionsThemes->isChecked())
    {
        QString warning = "";
        warning = tr("Ви впевнені, що хочете видалити тему \"") + ui->themesList->currentItem()->text() + "\"?\n";

        if(QMessageBox::question(this, tr("Підтвердіть Ваш вибір"), warning, tr("Так"), tr("Ні")) == 1)
            return;

        QSqlQuery q;

        q.prepare("UPDATE themes SET deleted = 1 WHERE id_theme = :id");
        q.bindValue(":id", ui->themesList->currentItem()->data(Qt::UserRole).toPoint().x());
        if(!q.exec())
        {
            throwDBError(q.lastError());
            return;
        }

        int curRow = ui->themesList->currentRow();
        updateLists();
        updateThemesList();
        if(curRow == 0)
            ui->themesList->setCurrentRow(0);
        else
            ui->themesList->setCurrentRow(curRow-1);
        QMessageBox::information(this, tr("Успіх"), tr("Обрана тема була успішно видалена"), tr("Гаразд"));
        emit on_rbActionsThemes_clicked();
        return;
    }
}

void HelpSubjects::on_multiDeleteButton_clicked()
{
    multipleDelete = !multipleDelete;
    if(multipleDelete)
    {
        ui->subjectsList->setSelectionMode(QAbstractItemView::MultiSelection);
        ui->sectionsList->setSelectionMode(QAbstractItemView::MultiSelection);
        ui->themesList->setSelectionMode(QAbstractItemView::MultiSelection);

        QString css = "QListWidget { border: 1px solid #E92929; background-color: #F7F7F7; }";
        ui->subjectsList->setStyleSheet(css);
        ui->sectionsList->setStyleSheet(css);
        ui->themesList->setStyleSheet(css);

        ui->multiDeleteButton->setText("Підтвердити");
        ui->closeButton->setText("Відмінити");
        ui->addButton->setEnabled(false);
        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);

        ui->subjectsList->setCurrentRow(-1);
        ui->sectionsList->setCurrentRow(-1);
        ui->themesList->setCurrentRow(-1);

    }
    else
    {
        if(QMessageBox::question(this, tr("Множинне видалення"), tr("Множинне видалення дає змогу видаляти декілька пунктів (предметів, розділів, тем) за одне нажаття кнопки.\n\nВи впевнені, що бажаєте видалити обрані пункти?"), tr("Так"), tr("Ні")) == 1)
        {
            multipleDelete = !multipleDelete;
            return;
        }

        for(int i = 0; i < ui->themesList->count(); i++)
            if(ui->themesList->item(i)->isSelected())
            {
                QSqlQuery q;
                q.prepare("UPDATE themes SET deleted = 1 WHERE id_theme = :id");
                q.bindValue(":id", ui->themesList->item(i)->data(Qt::UserRole).toPoint().x());
                if(!q.exec())
                {
                    throwDBError(q.lastError());
                    return;
                }
            }

        for(int i = 0; i < ui->sectionsList->count(); i++)
            if(ui->sectionsList->item(i)->isSelected())
            {
                QSqlQuery q;
                q.prepare("UPDATE sections SET deleted = 1 WHERE id_section = :id");
                q.bindValue(":id", ui->sectionsList->item(i)->data(Qt::UserRole).toPoint().x());
                if(!q.exec())
                {
                    throwDBError(q.lastError());
                    return;
                }

                q.prepare("UPDATE themes SET deleted = 1 WHERE id_section = :id AND deleted = 0");
                q.bindValue(":id", ui->sectionsList->item(i)->data(Qt::UserRole).toPoint().x());
                if(!q.exec())
                {
                    throwDBError(q.lastError());
                    return;
                }
            }

        for(int i = 0; i < ui->subjectsList->count(); i++)
            if(ui->subjectsList->item(i)->isSelected())
            {
                QSqlQuery mainQ;
                QSqlQuery helpQ;
                mainQ.prepare("UPDATE subjects SET deleted = 1 WHERE id_subject = :id");
                mainQ.bindValue(":id", ui->subjectsList->item(i)->data(Qt::UserRole).toInt());
                if(!mainQ.exec())
                {
                    throwDBError(mainQ.lastError());
                    return;
                }

                helpQ.prepare("SELECT id_section FROM sections WHERE id_subject = :id AND deleted = 0");
                helpQ.bindValue(":id", ui->subjectsList->item(i)->data(Qt::UserRole).toInt());
                if(!helpQ.exec())
                {
                    throwDBError(helpQ.lastError());
                    return;
                }

                while(helpQ.next())
                {
                    qDebug() << helpQ.value(0).toInt();
                    mainQ.prepare("UPDATE themes SET deleted = 1 WHERE id_section = :id");
                    mainQ.bindValue(":id", helpQ.value(0).toInt());
                    if(!mainQ.exec())
                    {
                        throwDBError(mainQ.lastError());
                        return;
                    }
                }

                mainQ.prepare("UPDATE sections SET deleted = 1 WHERE id_subject = :id");
                mainQ.bindValue(":id", ui->subjectsList->item(i)->data(Qt::UserRole).toInt());
                if(!mainQ.exec())
                {
                    throwDBError(mainQ.lastError());
                    return;
                }
            }

        ui->multiDeleteButton->setText("Множинне  \nвидалення");
        ui->closeButton->setText("Закрити");
        ui->subjectsList->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->sectionsList->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->themesList->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->subjectsList->setStyleSheet("");
        ui->sectionsList->setStyleSheet("");
        ui->themesList->setStyleSheet("");
        updateLists();
        updateSubjectsList();
        ui->rbActionsSubjects->setChecked(true);
        emit on_rbActionsSubjects_clicked();
        QMessageBox::information(this, tr("Успіх"), tr("Множинне видалення пройшло успішно!"), tr("Гаразд"));
    }
}

void HelpSubjects::on_themesList_doubleClicked(const QModelIndex &index)
{
    if(behavior && multipleDelete)
        return;

    if(behavior)
    {
        emit on_closeButton_clicked();
        return;
    }

    int dlgResult = QMessageBox::question(this, tr("Оберіть варіант"), tr("Бажаєте перейти до редагування питань?"), tr("Так"), tr("Ні"));

    if(dlgResult == 1)
        emit on_editButton_clicked();

    if(dlgResult == 0)
    {
        Questions *wnd = new Questions(this);
        connect(this, SIGNAL(sendData(int, QString, QString, QString, bool)), wnd, SLOT(recieveDataFromThemes(int, QString, QString, QString, bool)));
        emit sendData(ui->themesList->currentItem()->data(Qt::UserRole).toPoint().x(),
                      ui->subjectsList->currentItem()->text(),
                      ui->sectionsList->currentItem()->text(),
                      ui->themesList->currentItem()->text(),
                      false);
        /*wnd->setAttribute( Qt::WA_DeleteOnClose, true );
        wnd->show();*/
        if(!wnd->exec())
            delete wnd;
    }
}

void HelpSubjects::throwDBError(QSqlError error)
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
