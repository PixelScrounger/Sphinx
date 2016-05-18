#include "printquestionary.h"
#include "ui_printquestionary.h"

PrintQuestionary::PrintQuestionary(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintQuestionary)
{
    ui->setupUi(this);

    this->setWhatsThis(tr("Форма, що слугує для створення .pdf-файлів, які містять вміст тестувальника. А саме: список питань/відповідей, таблиця, у якій студент зазначає правильні відповіді, а також QR-код, який допомагає у розпізнаванні таблиці із відповідями."));
    ui->Questions->setWhatsThis(tr("Поле, в якому задається кількість питань, які виводитимуться на друк. У загальному, кількість питань не може перебільшувати 40 штук. Проте, при виборі тестувальника проводиться його аналіз, на основі якого ставиться ліміт на кількість, який дорівнює кількості зареєстрованих у тестувальнику питань. "));
    ui->Answers->setWhatsThis(tr("Поле, в якому задається кількість відповідей, які виводитимуться на друк. У загальному, кількість відповідей не може перебільшувати 5 штук. Проте, при виборі тестувальника проводиться його аналіз, на основі якого ставиться ліміт на кількість, який дорівнює мінімальній кількості відповідей серед усіх питань тестувальника. "));
    ui->Variants->setWhatsThis(tr("Поле, в якому задається кількість варіантів, що не може перевищувати позначки у 100."));
    ui->key->setWhatsThis(tr("Поле, у якому задається ключ для шифрування інформації, яка міститиметься у QR-коді. Ключ потрібно запам'ятати, оскільки, у майбутньому, таблиця із відповідями буде піддаватися комп'ютерному аналізу, і для її розпізнавання потрібна інформація із QR-кода. Оскільки ця інформації зашифрована, то спершу її потрібно перевести у зрозумілий для програми вигляд. Саме для цього слугує ключ."));
    ui->PrintButton->setWhatsThis(tr("Натисніть для створення файлів-тестувальників."));
    ui->lblMsg->setWhatsThis(tr("Напис, що містить інформацію про можливість друку обраного тестувальника.\n\nПід час вибору тестувальника, він піддається низкі перевірок. Наприклад, перевіряється мінімальна кількість питань у тестувальнику, яка має перевищувати позначку у 3. Також йде перевірка на те, чи всі питання містять правильні відповіді, і чи є у питаннь неправильні відповіді. \nТаким чином, мінімальна допустима комбінація із відповідей виглядає так: одна правильна та одна неправильна відповіді."));
    ui->lblStatus->setWhatsThis(tr("Напис, який повідомляє про те, який із файлів створюється у даний момент."));
    ui->progressBar->setWhatsThis(tr("Графічне зображення процесу створення файлів."));

    setModal(true);

    htmlString = "";
    topLeftCorner = QPoint(0,0);
    topRightCorner = QPoint(0,0);
    bottomLeftCorner = QPoint(0,0);
    ACount = 0;
    QCount = 0;

    idQuestionary = 0;

    printer = new QPrinter(QPrinter::HighResolution);
    printer->setPaperSize(QPrinter::A4);

    painter = new QPainter();

    document = new QTextDocument();
    document->setDefaultFont(QFont("Arial", 14));
    document->setDocumentMargin(300);

    situationalRandom = (SelectDatabase::getDBLocation() == true)?"RANDOM()":"RAND()";
}

PrintQuestionary::~PrintQuestionary()
{
    delete ui;
}

void PrintQuestionary::on_PrintButton_clicked()
{
    if(ui->key->text().isEmpty())
    {
        QMessageBox::information(this, tr("Введіть ключ"), tr("Для продовження друку введіть ключ"), tr("Гаразд"));
        ui->key->setFocus();
        return;
    }

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(ui->Variants->value() + 1);
    ui->progressBar->setValue(0);
    ui->lblStatus->setText(tr("Йде формування загального файлу..."));

    printer->setOutputFormat(QPrinter::PdfFormat);
    setupDocument();

    QString path = QString(QCoreApplication::applicationDirPath() + "/" + tr("Тестувальники") + "/" + subject + "/" + theme + "/" + nameQuestionary + "/" + Login::getUsername() + "/" + QDate::currentDate().toString("dd.MM.yyyy") + "/");

    QDir dpath(path);
    if(!dpath.exists(path))
        dpath.mkpath(path);

    if(QFileInfo(QString(path + tr("Загальний файл") + ".pdf")).exists() == false)
    {
        QFile pdfFile(QString(path + tr("Загальний файл") + ".pdf"));
        if(pdfFile.open(QIODevice::ReadWrite))
        {
            qDebug() << "File was opened";
            printer->setOutputFileName(QString(path + tr("Загальний файл") + ".pdf"));
            if(painter->begin(printer))
                qDebug() << "Printer began his job";
            if(painter->end())
                qDebug() << "Printer finished his job";
        }

        pdfFile.close();
        qDebug() << "File was closed";
    }
    else
        printer->setOutputFileName(QString(path + tr("Загальний файл") + ".pdf"));


    if (!painter->begin(printer))
    {
        qWarning("Printer error!");
        return;
    }

    ui->progressBar->setValue(1);

    QStringList htmlStrList;
    for(int variant = 1; variant <= ui->Variants->value(); variant++)
    {
        htmlString = createHTML(variant);
        htmlStrList << htmlString;
        document->setHtml(htmlString);

        QRect contentRect = QRect(QPoint(0, 0), document->size().toSize());
        QRect currentRect(0, 0, printer->pageRect().width(), printer->pageRect().height());

        while (currentRect.intersects(contentRect))
        {
            painter->save();
            painter->translate(0, -currentRect.y());
            document->drawContents(painter, currentRect);  // draws part of the document
            painter->restore();

            // Translate the current rectangle to the area to be printed for the next page
            currentRect.translate(0, currentRect.height());

            //Inserting a new page if there is still area left to be printed
            if (currentRect.intersects(contentRect))
                printer->newPage();
        }

        drawTable(QString::number(variant));
        drawMarks();
        drawQR(true, 0);

        if(variant != ui->Variants->value())
            printer->newPage();
    }
    painter->end();

    for(int variant = 1; variant <= ui->Variants->value(); variant++)
    {
        ui->lblStatus->setText(tr("Йде формування файлу із ") + QString::number(variant) + tr(" варіантом"));
        ui->progressBar->setValue(variant+1);
        printToPDF(QString::number(variant), htmlStrList[variant-1]);
    }

    ui->lblStatus->setText(tr("Формування завершене. Тестувальники готові"));
    htmlString = "";
    if(QMessageBox::question(this, tr("Операція успішна"), tr("Формування файлів успішно завершене. Бажаєте відкрити теку розташування файлів?"), tr("Так"), tr("Ні")) == 0)
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void PrintQuestionary::recieveDataFromQuestionaryList(int id, QString name, QString subj, QString thme)
{
    idQuestionary = id;
    nameQuestionary = name;
    subject = subj;
    theme = thme;
    ui->lblHeader->setText(tr("Друк тестувальника під назвою «") + nameQuestionary + "»");
    checkAccessibility();
}

void PrintQuestionary::checkAccessibility()
{
    QSqlQuery qQuestions;
    QSqlQuery qAnswers;
    QSqlQuery tempQ;
    int minAnswersCount = 5;
    int temp = 0;
    int qQuestionsSize = 0;
    int qAnswersSize = 0;

    qQuestions.prepare("SELECT id_question FROM questions_questionary WHERE id_questionary = :id");
    qQuestions.bindValue(":id", idQuestionary);
    qDebug() << "ID = " << idQuestionary;

    if(!qQuestions.exec())
    {
        throwDBError(qQuestions.lastError());
        ui->PrintButton->setEnabled(false);
        return;
    }

    qQuestionsSize = MainWindow::qSize(qQuestions);
    while(qQuestions.previous()) {}

    while(qQuestions.next())
    {
        qDebug() << "QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ" << qQuestions.value(0).toInt();
        temp = 0;
//*********************************************************************************
        qAnswers.prepare("SELECT id_answer FROM answers WHERE id_question = :id AND correct = 1 AND deleted = 0 LIMIT 1");
        qAnswers.bindValue(":id", qQuestions.value(0).toInt());
        if(!qAnswers.exec())
        {
            throwDBError(qAnswers.lastError());
            ui->PrintButton->setEnabled(false);
            return;
        }
        qAnswersSize = MainWindow::qSize(qAnswers);
        if(qAnswersSize < 1)
        {
            tempQ.prepare("SELECT text_question FROM questions WHERE id_question = :id");
            tempQ.bindValue(":id", qQuestions.value(0).toInt());
            if(!tempQ.exec())
            {
                throwDBError(tempQ.lastError());
                ui->PrintButton->setEnabled(false);
                return;
            }
            tempQ.first();
            ui->lblMsg->setStyleSheet("color: red;");
            ui->lblMsg->setText(QString("Друк неможливий, оскільки питання \"" + tempQ.value(0).toString() + "\" не має жодної правильної відповіді"));
            ui->PrintButton->setEnabled(false);
            return;
        }
        temp += qAnswersSize;
//*********************************************************************************
        qAnswers.prepare("SELECT id_answer FROM answers WHERE id_question = :id AND correct = 0 AND deleted = 0 LIMIT 1");
        qAnswers.bindValue(":id", qQuestions.value(0).toInt());
        if(!qAnswers.exec())
        {
            throwDBError(qAnswers.lastError());
            ui->PrintButton->setEnabled(false);
            return;
        }
        if(MainWindow::qSize(qAnswers) < 1)
        {
            tempQ.prepare("SELECT text_question FROM questions WHERE id_question = :id");
            tempQ.bindValue(":id", qQuestions.value(0).toInt());
            if(!tempQ.exec())
            {
                throwDBError(tempQ.lastError());
                ui->PrintButton->setEnabled(false);
                return;
            }
            tempQ.first();
            ui->lblMsg->setStyleSheet("color: red;");
            ui->lblMsg->setText(QString("Друк неможливий, оскільки питання \"" + tempQ.value(0).toString() + "\" не має жодної неправильної відповіді"));
            ui->PrintButton->setEnabled(false);
            return;
        }
//*********************************************************************************
        qAnswers.prepare("SELECT count(id_answer) FROM answers WHERE id_question = :id AND correct = 0 AND deleted = 0 LIMIT 4");
        qAnswers.bindValue(":id", qQuestions.value(0).toInt());
        if(!qAnswers.exec())
        {
            throwDBError(qAnswers.lastError());
            ui->PrintButton->setEnabled(false);
            return;
        }
        qAnswers.first();
        temp += qAnswers.value(0).toInt();

        if(temp < minAnswersCount)
            minAnswersCount = temp;

    }
    qDebug() << "qQuestions.size() = " << qQuestionsSize;

    ui->Questions->setMaximum((qQuestionsSize > 40)?40:qQuestionsSize);
    ui->Questions->setValue((qQuestionsSize > 40)?40:qQuestionsSize);
    ui->lblMaxQuestions->setText(QString("Кількість запитань (максимум - %1)").arg(QString::number((qQuestionsSize > 40)?40:qQuestionsSize)));

    ui->Answers->setMaximum((minAnswersCount > 5)?5:minAnswersCount);
    ui->Answers->setValue((minAnswersCount > 5)?5:minAnswersCount);
    ui->lblMaxAnswers->setText(QString("Кількість відповідей (максимум - %1)").arg(QString::number((minAnswersCount > 5)?5:minAnswersCount)));

    qDebug() << "minAnswersCount = " << minAnswersCount;
    if(qQuestionsSize < 3)
    {
        ui->lblMsg->setStyleSheet("color: red;");
        ui->lblMsg->setText(tr("Друк неможливий, оскільки тестувальник повинен містити хоча б 3 питання"));
    }
    else
    {
        ui->lblMsg->setStyleSheet("color: green;");
        ui->lblMsg->setText(tr("Тестувальник готовий для друку"));
    }
}

void PrintQuestionary::throwDBError(QSqlError error)
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

void PrintQuestionary::setupDocument()
{
    document->setPageSize(printer->pageRect().size());
    document->documentLayout()->setPaintDevice(printer);
}

void PrintQuestionary::drawTable(QString variant)
{
    painter->setFont(QFont("Arial", 10, QFont::DemiBold));
    const int yStart = (theme.length() > 40)?4500:4270;
    const int xStart = 570;
    const int width = 400;
    const int height = 400;
    int curX = xStart, curY = yStart;

    topLeftCorner = QPoint(curX, curY);

    QCount = ui->Questions->value();
    ACount = ui->Answers->value();
    /*QCount = 35;
    ACount = 5;*/

    printer->newPage();

    QRect rect;
    QPen blackPen;
    QBrush whiteBr(Qt::white);
    blackPen.setWidth(20);
    blackPen.setColor(Qt::black);
    painter->setPen(blackPen);
    painter->setBrush(whiteBr);

    for (int i = 0; i <= ACount; i++)
    {
        //Перший стовбець
        rect.setRect(curX, curY, width, height);
        painter->drawRect(rect);
        if(i == 0) painter->drawText(rect, Qt::AlignCenter, "№");
        if(i == 1) painter->drawText(rect, Qt::AlignCenter, tr("А"));
        if(i == 2) painter->drawText(rect, Qt::AlignCenter, tr("Б"));
        if(i == 3) painter->drawText(rect, Qt::AlignCenter, tr("В"));
        if(i == 4) painter->drawText(rect, Qt::AlignCenter, tr("Г"));
        if(i == 5) painter->drawText(rect, Qt::AlignCenter, tr("Д"));
        curY += height;
    }

    if(QCount > 20)
    {
        curY += height;
        for (int i = 0; i <= ACount; i++)
        {
            //Перший стовбець
            rect.setRect(curX, curY, width, height);
            painter->drawRect(rect);
            if(i == 0) painter->drawText(rect, Qt::AlignCenter, "№");
            if(i == 1) painter->drawText(rect, Qt::AlignCenter, tr("А"));
            if(i == 2) painter->drawText(rect, Qt::AlignCenter, tr("Б"));
            if(i == 3) painter->drawText(rect, Qt::AlignCenter, tr("В"));
            if(i == 4) painter->drawText(rect, Qt::AlignCenter, tr("Г"));
            if(i == 5) painter->drawText(rect, Qt::AlignCenter, tr("Д"));
            curY += height;
        }
    }

    bottomLeftCorner = QPoint(curX, curY);

    curX += width;
    curY = yStart;

    for(int i = 0; i < ((QCount>20)?20:QCount); i++)
    {
        for (int j = 0; j <= ACount; j++)
        {
            rect.setRect(curX, curY, width, height);
            painter->drawRect(rect);
            if(j == 0) painter->drawText(rect, Qt::AlignCenter, QString::number(i+1));
            curY += height;
        }
        curX += width;
        curY = yStart;
    }

    topRightCorner = QPoint(curX, curY);

    if(QCount > 20)
    {
        int temp = yStart + (height * (ACount + 2));
        qDebug() << temp;
        curX = xStart + width;
        curY = temp;
        for(int i = 20; i < QCount; i++)
        {
            for (int j = 0; j <= ACount; j++)
            {
                rect.setRect(curX, curY, width, height);
                painter->drawRect(rect);
                if(j == 0) painter->drawText(rect, Qt::AlignCenter, QString::number(i+1));
                curY += height;
            }
            curX += width;
            curY = temp;
        }
    }

    curY = 500;
    painter->setFont(QFont("Arial", 20, QFont::Bold));
    rect.setRect(150,curY,printer->width()-300,(theme.length() > 40)?700:450);
    painter->drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, "Тема: «" + theme + "»");

    curY += (theme.length() > 40)?850:570;
    painter->setFont(QFont("Arial", 18, QFont::Bold));
    rect.setRect(150,curY,printer->width()-300,300);
    painter->drawText(rect, Qt::AlignCenter, "Варінат №" + variant);

    curY += 900;
    painter->setFont(QFont("Arial", 14, QFont::Normal));
    rect.setRect(600,curY,printer->width()/2,300);
    painter->drawText(rect, Qt::AlignLeft, "Група: ____________");

    curY += 500;
    rect.setRect(600,curY,printer->width()/1.5,300);
    painter->drawText(rect, Qt::AlignLeft, "Прізвище та ініціали: ______________________");

    curY += 875;
    painter->setFont(QFont("Arial", 18, QFont::Bold));
    rect.setRect(150,curY,printer->width()-300,300);
    painter->drawText(rect, Qt::AlignCenter, "Відповіді");

}

void PrintQuestionary::drawMarks()
{
    QBrush blackBr(Qt::black);
    QBrush whiteBr(Qt::white);

    painter->setBrush(blackBr);
    painter->setPen(Qt::black);

    painter->drawEllipse(QPoint(topLeftCorner.x() - 200, topLeftCorner.y() - 200), 200, 200);
    painter->drawEllipse(QPoint(topRightCorner.x() + 200, topRightCorner.y() - 200), 200, 200);
    painter->drawEllipse(QPoint(bottomLeftCorner.x() - 200, bottomLeftCorner.y() + 200), 200, 200);

    painter->setBrush(whiteBr);
    painter->setPen(Qt::white);

    painter->drawEllipse(QPoint(topLeftCorner.x() - 200, topLeftCorner.y() - 200), 150, 150);
    painter->drawEllipse(QPoint(topRightCorner.x() + 200, topRightCorner.y() - 200), 150, 150);
    painter->drawEllipse(QPoint(bottomLeftCorner.x() - 200, bottomLeftCorner.y() + 200), 150, 150);

    painter->setBrush(blackBr);
    painter->setPen(Qt::black);

    painter->drawEllipse(QPoint(topLeftCorner.x() - 200, topLeftCorner.y() - 200), 85, 85);
    painter->drawEllipse(QPoint(topRightCorner.x() + 200, topRightCorner.y() - 200), 85, 85);
    painter->drawEllipse(QPoint(bottomLeftCorner.x() - 200, bottomLeftCorner.y() + 200), 85, 85);

    painter->setBackground(whiteBr);
}

void PrintQuestionary::drawQR(bool behavior = true, int k = 0)
{
    QString TempStr;
    for(int i = 0; i < QRVector.size(); i++)
        TempStr.append(QString::number(QRVector[i]));

    questions data;
    if(behavior)
    {
        data.random[0] = qrand() % 200;
        data.random[1] = qrand() % 200;
        data.q_count = ui->Questions->value();
        data.a_count = ui->Answers->value();
        data.crc32 = 0;
        for(int i = 0; i < QRVector.size(); i++)
            data.answers[i] = QRVector[i];
        data.crc32 = MainWindow::crc32(0, &data, sizeof(data));
        qDebug() << "crc32 = " << data.crc32;
        QRDataVector.append(data);
    }
    else
    {
        data = QRDataVector[k];
    }

    unsigned char *p_struct = (unsigned char *) &data;
//---------------key-----------------------------
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(ui->key->text().toLatin1());
    QString hashStr(hash.result().toHex());
    qDebug() << "hashStr =" << hashStr;

    char *passwd = new char[hashStr.length()+1];
    strcpy(passwd, hashStr.toStdString().c_str());
//---------------key-----------------------------

    qDebug() << "Original data:";
    QString temp;
    for(unsigned int i = 0; i < sizeof(questions); i++)
        temp.append(QString::number((unsigned int) p_struct[i]) + " ");
    qDebug() << temp;
    temp.clear();

    unsigned char *encrypted_data = (unsigned char *)malloc(sizeof(questions));
    memcpy(encrypted_data, &data, sizeof(questions));

    unsigned char oldval = 0xff;
    unsigned int j = 0;
    for(size_t i = 0; i < sizeof(questions); i++)
    {
        unsigned char newval = oldval ^ encrypted_data[i];
        newval ^= passwd[j];
        oldval = newval;
        encrypted_data[i] = newval;
        j++;
        if(j > strlen(passwd)) j = 0;
    }

    qDebug() << "Encrypted data:";
    for(unsigned int i = 0; i < sizeof(questions); i++)
        temp.append(QString::number((unsigned int) encrypted_data[i]) + " ");
    qDebug() << temp;

#ifdef Q_OS_WIN

    QQREncode encoder;
    encoder.encode(temp);
    QImage image = encoder.toQImage().scaled(1500,1500);
    //image.save("temp.png",0,100);

    QPixmap pix = QPixmap::fromImage(image);
    image = pix.toImage();
    painter->drawImage(printer->width()/1.4,topLeftCorner.y() - 2550,image);

#else

    QImage image(100, 100, QImage::Format_RGB32);
    linuxDrawImage(&image, temp);
    painter->drawImage(printer->width()/1.4,topLeftCorner.y() - 2550,image.scaled(1250,1250));

#endif

    temp.clear();
}

#ifdef Q_OS_LINUX
void PrintQuestionary::linuxDrawImage(QImage *image, QString QRText)
{
    QRcode *qr = QRcode_encodeString(QRText.toStdString().c_str(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
    qDebug() << qr->width;
    if(0 != qr)
    {
        QPainter painter(image);
        QColor fg("black");
        QColor bg("white");

        painter.setBrush(bg);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0,0,image->width(), image->height());
        painter.setBrush(fg);
        const int s = (qr->width > 0)?qr->width:1;
        const double w = 100;
        const double h = 100;
        const double aspect = w/h;
        const double scale = ((aspect > 1.0)?h:w)/s;
        for(int y = 0; y < s; y++)
        {
            const int yy = y*s;
            for(int x = 0; x < s; x++)
            {
                const int xx = yy + x;
                const unsigned char b = qr->data[xx];
                if(b & 0x01)
                {
                    const double rx1 = x * scale, ry1 = y * scale;
                    QRectF r(rx1, ry1, scale, scale);
                    painter.drawRects(&r, 1);
                }
            }
        }
        QRcode_free(qr);
    }
    else
    {
        QPainter painter(image);
        painter.begin(image);
        QColor error("red");
        painter.setBrush(error);
        painter.drawRect(0, 0, image->width(), image->height());
        qDebug()<<"QR FAIL: "<< strerror(errno);
        painter.end();
    }
    qr = 0;
}
#endif

void PrintQuestionary::printToPDF(QString variant, QString html)
{
    document->setHtml(html);

    QString path = QString(QCoreApplication::applicationDirPath() + "/" + tr("Тестувальники") + "/" + subject + "/" + theme + "/" + nameQuestionary + "/" + Login::getUsername() + "/" + QDate::currentDate().toString("dd.MM.yyyy") + "/");

    QDir dpath(path);
    if(!dpath.exists(path))
        dpath.mkpath(path);

    if(QFileInfo(QString(path + variant + ".pdf")).exists() == false)
    {
        QFile pdfFile(QString(path + variant + ".pdf"));
        if(pdfFile.open(QIODevice::ReadWrite))
        {
            qDebug() << "File was opened";
            printer->setOutputFileName(QString(path + variant + ".pdf"));
            if(painter->begin(printer))
                qDebug() << "Printer began his job";
            if(painter->end())
                qDebug() << "Printer finished his job";
        }

        pdfFile.close();
        qDebug() << "File was closed";
    }
    else
    {
        qDebug() << "File already exist";
        printer->setOutputFileName(QString(path + variant + ".pdf"));
    }

    if (!painter->begin(printer))
    {
        qWarning("Printer error!");
        return;
    }

    QRect contentRect = QRect(QPoint(0, 0), document->size().toSize());
    QRect currentRect(0, 0, printer->pageRect().width(), printer->pageRect().height());

    while (currentRect.intersects(contentRect))
    {
        painter->save();
        painter->translate(0, -currentRect.y());
        document->drawContents(painter, currentRect);  // draws part of the document
        painter->restore();

        // Translate the current rectangle to the area to be printed for the next page
        currentRect.translate(0, currentRect.height());

        //Inserting a new page if there is still area left to be printed
        if (currentRect.intersects(contentRect))
            printer->newPage();
    }

    drawTable(variant);
    drawMarks();
    drawQR(false, variant.toInt() - 1);
    painter->end();
}

QString PrintQuestionary::createHTML(int variant)
{
    QSqlQuery questionIDQuery;
    QSqlQuery questionTextQuery;
    QSqlQuery answerIDQuery;
    QSqlQuery answerTextQuery;
    Answer tAnswer;
    int questionsCounter = 1;

    QRVector.clear();

    QString temp = "";
    temp += "<HTML>";

    temp += "<HEAD>";
    temp += setCSS();
    temp += "</HEAD>";

    temp += "<BODY>";
    temp += "<P align='center' id='header'> Тема: «";       //Заголовок
    temp += theme;
    temp += "»</P>";
    temp += "<P align='center' id='variant'> Варіант №";      //Варіант
    temp += QString::number(variant);
    temp += "</P>";

    //Заповнення питаннями
    questionIDQuery.prepare(QString("SELECT id_question from questions_questionary WHERE id_questionary = :id ORDER BY %1 LIMIT :limitQuestions")
                            .arg(situationalRandom));
    questionIDQuery.bindValue(":id", idQuestionary);
    questionIDQuery.bindValue(":limitQuestions", ui->Questions->value());
    if(questionIDQuery.exec())
    {
        while(questionIDQuery.next())
        {
            questionsCounter;
            //Вибірка тексту одного питання, і його добавлення у htmlString
            questionTextQuery.prepare("SELECT text_question FROM questions WHERE id_question = :id");
            questionTextQuery.bindValue(":id", questionIDQuery.value(0).toInt());
            if(questionTextQuery.exec())
            {
                questionTextQuery.first();
                temp += "<DIV id='question'>";
                temp += QString::number(questionsCounter) + ". " + questionTextQuery.value(0).toString();
                temp += "</DIV>";
            }
            questionsCounter++;
            indexVector.clear();
            shuffleVector.clear();

            //Вибір одної правильної відповіді, і її внесення у масив indexVector
            answerIDQuery.prepare(QString("SELECT id_answer FROM answers WHERE id_question = :id AND correct = 1 AND deleted = 0 ORDER BY %1 LIMIT 1")
                                  .arg(situationalRandom));
            answerIDQuery.bindValue(":id", questionIDQuery.value(0).toInt());
            if(answerIDQuery.exec())
                if(answerIDQuery.first())
                {
                    tAnswer.id = answerIDQuery.value(0).toInt();
                    tAnswer.correct = true;
                    indexVector.append(tAnswer);
                }

            //Вибір неправильних відповідей у кількості ui->Answers->value() - 1, і їх внесення у масив indexVector
            answerIDQuery.prepare(QString("SELECT id_answer FROM answers WHERE id_question = :id AND correct = 0 AND deleted = 0 ORDER BY %1 LIMIT :limitAnswers")
                                  .arg(situationalRandom));
            answerIDQuery.bindValue(":id", questionIDQuery.value(0).toInt());
            answerIDQuery.bindValue(":limitAnswers", ui->Answers->value() - 1);
            if(answerIDQuery.exec())
                while(answerIDQuery.next())
                {
                    tAnswer.id = answerIDQuery.value(0).toInt();
                    tAnswer.correct = false;
                    indexVector.append(tAnswer);
                }

            /*qDebug() << "-----------------------------------------";
            for(int i = 0; i < indexVector.size(); i++)
                qDebug() << indexVector[i].correct << " " << indexVector[i].id;*/

            //Перемішування масиву
            int randomIndex = 0;
            while(shuffleVector.size() != indexVector.size())
            {
                randomIndex = qrand() % indexVector.size();

                if(shuffleVector.isEmpty())
                {
                    shuffleVector.append(indexVector[randomIndex]);
                    continue;
                }

                if(!hasElement(indexVector[randomIndex].id))
                    shuffleVector.append(indexVector[randomIndex]);
            }

            fillQRVector();
            /*qDebug() << "-----------------------------------------";
            for(int i = 0; i < shuffleVector.size(); i++)
                qDebug() << shuffleVector[i].correct << " " << shuffleVector[i].id;*/
            QStringList ans;
            ans << tr("A") << tr("Б") << tr("В") << tr("Г") << tr("Д");
            for(int i = 0; i < shuffleVector.size(); i++)
            {
                answerTextQuery.prepare("SELECT text_answer from answers where id_answer = :id");
                answerTextQuery.bindValue(":id", shuffleVector[i].id);
                if(answerTextQuery.exec())
                {
                    while(answerTextQuery.next())
                    {
                        temp += "<DIV id='answers'>";
                        temp += ans[i] + ") " + answerTextQuery.value(0).toString();
                        temp += "</DIV>";
                    }

                }
                else
                    qDebug() << "answerTextQuery error: " << answerTextQuery.lastError();
            }
        }
    }
    else
    {
        qDebug() << "Can't exec questionIDQuery";
        return "";
    }

    temp += "</BODY>";

    temp += "</HTML>";
    return temp;
}

bool PrintQuestionary::hasElement(int id)
{
    for(int i = 0; i < shuffleVector.size(); i++)
        if(shuffleVector[i].id == id)
            return true;
    return false;
}

void PrintQuestionary::fillQRVector()
{
    for(int i = 0; i < shuffleVector.size(); i++)
    {
        if(shuffleVector[i].correct)
        {
            QRVector.append(i+1);
            break;
        }
    }
}

QByteArray PrintQuestionary::encrypt(QByteArray text, quint32 key)
{
    QByteArray beginArr(text);

    for(int i = 0; i < beginArr.size(); i++)
        if(i == 0)
            beginArr[i] = beginArr[i] ^ key;
        else
            beginArr[i] = beginArr[i] ^ key ^ beginArr[i-1];

    return beginArr;
}

QByteArray PrintQuestionary::decrypt(QByteArray text, quint32 key)
{
    QByteArray beginArr(text);
    QByteArray endArr;

    for(int i = beginArr.size()-1; i >= 0; i--)
        if(i == 0)
            endArr[i] = beginArr[i] ^ key;
        else
            endArr[i] = beginArr[i] ^ beginArr[i-1] ^ key;

    return endArr;
}

QString PrintQuestionary::setCSS()
{
    return "<STYLE>                 \
        #header {                   \
            margin-bottom: 0em;     \
            font-size: 325px;       \
            font-weight: bold;      \
        }                           \
                                    \
        #variant {                  \
            font-size: 275px;       \
            font-weight: bold;      \
        }                           \
                                    \
        #question {                 \
            margin-top:10px;        \
            margin-bottom:5px;      \
        }                           \
                                    \
        #answers {                  \
            margin-left: 200px;     \
            margin-top: 0px;        \
            margin-bottom:3px;      \
        }                           \
                                    \
            </STYLE>;";
}
