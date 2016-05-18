#include "mymsgbox.h"

void MyMsgBox::setDetailedText( const QString& text ){
    QMessageBox::setDetailedText(text);
    details = NULL;
}

void MyMsgBox::setDetailsButtonText( const QString& t1, const QString& t2 ){
    _t1 = t1;
    _t2 = t2;
}

void MyMsgBox::detailsClicked(){
    QAbstractButton * btn = qobject_cast<QAbstractButton*>(sender());
    if( btn ){
        if( btn->text() == tr("Show Details...") and _t1!=QString() ){
            btn->setText(_t1);
        } else if( _t2!=QString() ){
            btn->setText(_t2);
        }
    }
}

void MyMsgBox::showEvent( QShowEvent * event ){
    QMessageBox::showEvent(event);
    if( details == NULL ){
        foreach( QAbstractButton * btn, this->buttons() ){
            if( buttonRole(btn) == QMessageBox::ActionRole and btn->text() == QMessageBox::tr("Show Details...") ){
                details = btn;
                connect(details, SIGNAL(clicked()), this, SLOT(detailsClicked()));
                if( _t1 != QString() ){
                    details->setText(_t1);
                }
                break;
            }
        }
    }
}

