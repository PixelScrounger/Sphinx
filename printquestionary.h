#ifndef PRINTQUESTIONARY_H
#define PRINTQUESTIONARY_H

#include <QDialog>
#include "mainwindow.h"
#include <cstdlib>

namespace Ui {
class PrintQuestionary;
}

class PrintQuestionary : public QDialog
{
    Q_OBJECT

public:
    explicit PrintQuestionary(QWidget *parent = 0);
    ~PrintQuestionary();

private slots:
    void on_PrintButton_clicked();

    void recieveDataFromQuestionaryList(int id, QString name, QString subj, QString thme);

    void checkAccessibility();

    void throwDBError(QSqlError error);

private:
    Ui::PrintQuestionary *ui;

    int idQuestionary;
    QString nameQuestionary;
    QString subject;
    QString theme;

    QString situationalRandom;
    struct Answer
    {
        int id;
        bool correct;
    };

    struct questions{
        unsigned char random[2];
        unsigned char q_count;
        unsigned char a_count;
        unsigned char answers[41];
        unsigned int crc32;
    };

    void setupDocument();
    void drawTable(QString variant);
    void drawMarks();
    void drawQR(bool behavior, int k);
#ifdef Q_OS_LINUX
    void linuxDrawImage(QImage *image, QString QRText);
#endif
    void printToPDF(QString variant, QString html);
    QString createHTML(int variant);
    QString setCSS();
    bool hasElement(int id);
    void fillQRVector();

    QString htmlString;
    QPrinter *printer;
    QPainter *painter;
    QTextDocument *document;
    QVector<Answer> indexVector;
    QVector<Answer> shuffleVector;
    QVector<int> QRVector;
    QVector<questions> QRDataVector;

    QPoint topLeftCorner;
    QPoint topRightCorner;
    QPoint bottomLeftCorner;

    int QCount;
    int ACount;

    QByteArray encrypt(QByteArray text, quint32 key);
    QByteArray decrypt(QByteArray text, quint32 key);
};

#endif // PRINTQUESTIONARY_H
