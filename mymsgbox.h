#ifndef MYMSGBOX_H
#define MYMSGBOX_H
#include <QMessageBox>
#include <QAbstractButton>


class MyMsgBox : public QMessageBox{
    Q_OBJECT
public:
    MyMsgBox( QWidget * parent = NULL ) : QMessageBox(parent){
        details = NULL;
    }
    MyMsgBox(  Icon icon, const QString & title, const QString & text, StandardButtons buttons = NoButton, QWidget * parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint )
    :QMessageBox(icon,title,text,buttons,parent,f)
    {
        details = NULL;
    }
    void setDetailedText( const QString& text );
    void setDetailsButtonText( const QString& t1, const QString& t2 );
protected:
    virtual void showEvent( QShowEvent * event );
protected slots:
    void detailsClicked();
protected:
    QString _t1, _t2;
    QAbstractButton * details;
};

#endif // MYMSGBOX_H
