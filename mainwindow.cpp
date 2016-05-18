#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);

    isLoginOpened = false;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    ui->adminButton->setEnabled((Login::getUsername() == "root")?true:false);
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    settings = new QSettings(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mpos = event->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton)
    {
        QPoint diff = event->pos() - mpos;
        QPoint newpos = this->pos() + diff;

        this->move(newpos);
    }
}

bool MainWindow::checkMatch(QString value, QString tableName, QString columnName)
{
    QSqlQuery q;
    QString str = QString("select * from %1 where %2 = '%3' and deleted = 0")
            .arg(tableName)
            .arg(columnName)
            .arg(value);
    q.exec(str);
    qDebug() << q.lastQuery();
    if(MainWindow::qSize(q) > 0)
        return false;
    return true;
}

bool MainWindow::checkMatch(QString value, QString tableName, QString columnName, QString idColumn ,int id)
{
    QSqlQuery q;
    QString str = QString("select * from %1 where %2 = '%3' and %4 = '%5' and deleted = 0")
            .arg(tableName)
            .arg(columnName)
            .arg(value)
            .arg(idColumn)
            .arg(id);
    q.exec(str);
    qDebug() << q.lastQuery();
    if(MainWindow::qSize(q) > 0)
        return false;
    return true;
}

bool MainWindow::checkRecordsCount(QString columnName, QString tableName)
{
    QSqlQuery q;
    QString s = QString("SELECT %1 FROM %2 WHERE deleted = 0").arg(columnName, tableName);;
    q.exec(s);

    if(MainWindow::qSize(q) > 0)
        return true;
    return false;
}

bool MainWindow::checkRecordsCount(QString columnName, QString tableName, QString idColumn, int id)
{
    QSqlQuery q;
    QString str = QString("select %1 from %2 where deleted = 0 and %3 = %4 limit 1")
            .arg(columnName)
            .arg(tableName)
            .arg(idColumn)
            .arg(id);
    q.exec(str);

    if(MainWindow::qSize(q) > 0)
        return true;
    return false;
}

int MainWindow::qSize(QSqlQuery q)
{
    q.last();
    int c = q.at() + 1;
    return (c < 0)?0:c;
}

unsigned int MainWindow::crc32(unsigned int crc, const void *buf, size_t size)
{
    const unsigned char *p;

    p = (unsigned char *)buf;
    crc = crc ^ ~0U;

    while (size--)
    crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    return crc ^ ~0U;
}

void MainWindow::on_themesButton_clicked()
{
    HelpSubjects *w = new HelpSubjects(this);
    w->show();
    if(!w->exec() && !isLoginOpened)
        delete w;

}

void MainWindow::on_questionaryListButton_clicked()
{
    QuestionaryList *w = new QuestionaryList(this);
    w->show();
    if(!w->exec() && !isLoginOpened)
        delete w;
}

void MainWindow::on_questionsButton_clicked()
{
    Questions *w = new Questions(this);
    w->show();
    if(!w->exec() && !isLoginOpened)
        delete w;
}

void MainWindow::on_closeButton_clicked()
{
    QApplication::quit();
}

void MainWindow::on_openLoginButton_clicked()
{
    isLoginOpened = true;
    emit openLogin();
    this->close();
}

void MainWindow::on_adminButton_clicked()
{
    Users *w = new Users(this);
    w->show();
    if(!w->exec() && !isLoginOpened)
        delete w;
}

void MainWindow::on_addQuestionaryButton_clicked()
{
    Questionary *w = new Questionary(this);
    w->show();
    if(!w->exec())
        delete w;
}

void MainWindow::on_scanButton_clicked()
{
    QString programPath = settings->value("ScanningProgramPath").toString();
    if(programPath.isEmpty())
    {
        programPath = QFileDialog::getOpenFileName(this, tr("Вкажіть шлях до файлу"), qApp->applicationDirPath(), tr("Sphinx.exe (*.exe)"));
        if(programPath.isEmpty())
            return;
        settings->setValue("ScanningProgramPath",programPath);
    }

    if(!QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(programPath))))
    {
        if(QMessageBox::critical(this, tr("Помилка"), tr("Не вдалося відкрити файл. Бажаєте вказати інший?"), tr("Так"), tr("Ні")) == 0)
        {
            programPath = QFileDialog::getOpenFileName(this, tr("Вкажіть шлях до файлу"), qApp->applicationDirPath(), tr("Sphinx.exe (*.exe)"));
            settings->setValue("ScanningProgramPath",programPath);
            emit on_scanButton_clicked();
        }
    }
}

void MainWindow::on_clearPath_clicked()
{
    settings->setValue("ScanningProgramPath", "");
    if(QMessageBox::question(this, tr("Успіх"), tr("Шлях до виконуваного файлу був успішно видалений. Бажаєте вказати новий?"), tr("Так"), tr("Ні")) == 0)
        emit on_scanButton_clicked();
}

void MainWindow::on_aboutButton_clicked()
{
    About *w = new About(this);
    w->show();
    if(!w->exec())
        delete w;
}
