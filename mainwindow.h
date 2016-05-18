#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtSql>

#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QFile>
#include <QCryptographicHash>

#include <QImage>
#include <QtPrintSupport/QtPrintSupport>

#ifdef Q_OS_LINUX
#include "qrencode.h"
#else
#include "qqrencode.h"
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <zbar.h>

#include "helpsubjects.h"
#include "addquestion.h"
#include "login.h"
#include "questions.h"
#include "questionary.h"
#include "questionarylist.h"
#include "printquestionary.h"
#include "users.h"
#include "about.h"

using namespace zbar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static bool checkMatch(QString value, QString tableName, QString columnName);
    static bool checkMatch(QString value, QString tableName, QString columnName, QString idColumn,int id);
    static bool checkRecordsCount(QString columnName, QString tableName);
    static bool checkRecordsCount(QString columnName, QString tableName, QString idColumn, int id);

    static int qSize(QSqlQuery q);
    static unsigned int crc32(unsigned int crc, const void *buf, size_t size);
    static unsigned int crc32_tab[];

signals:
    void openLogin();

private slots:

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void on_themesButton_clicked();

    void on_questionaryListButton_clicked();

    void on_questionsButton_clicked();

    void on_closeButton_clicked();

    void on_openLoginButton_clicked();

    void on_adminButton_clicked();

    void on_addQuestionaryButton_clicked();
    void on_scanButton_clicked();

    void on_clearPath_clicked();

    void on_aboutButton_clicked();

private:

    Ui::MainWindow *ui;
    QPoint mpos;
    bool isLoginOpened;
    QSettings *settings;
};

#endif // MAINWINDOW_H

