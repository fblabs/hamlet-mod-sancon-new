#include "hnewanagrafica.h"
#include "ui_hnewanagrafica.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>

HNewAnagrafica::HNewAnagrafica(QSqlDatabase pdb,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HNewAnagrafica)
{
    ui->setupUi(this);

    db=pdb;
}

HNewAnagrafica::~HNewAnagrafica()
{
    delete ui;
}


void HNewAnagrafica::on_pushButton_clicked()
{
    QSqlQuery q(db);

    QString SRagSoc=ui->leRagsoc->text();
    if(SRagSoc.length()<1){

        QMessageBox::warning(this,QApplication::applicationName(),"Attenzione, la ragione sociale non puo' essere vuota",QMessageBox::Ok);
        return;
    }
    QString sIndirizzo=ui->leAddress->text();
    QString sCity=ui->leCity->text();
    QString sCap=ui->leCap->text();
    QString sProv=ui->leProvincia->text();
    QString sNaz=ui->leNazione->text();
    QString sTel=ui->leTel->text();
    QString sContact=ui->leContatto->text();
    QString sCliente,sFornitore,sTrasportatore,sStampatore=QString();
    QString sNote=ui->leNote->text();

     if(ui->cbCliente->isChecked())
     {
         sCliente="1";
     }
     else
     {
         sCliente="0";
     }
     if(ui->cbfornitore->isChecked())
     {
         sFornitore="1";
     }
     else
     {
         sFornitore="0";
     }
    if(ui->cbTrasportatore->isChecked())
    {
         sTrasportatore="1";
    }
    else
    {
         sTrasportatore="0";
    }
    if(ui->cbStampatore->isChecked())
    {
         sStampatore="1";
    }
    else
    {
         sStampatore="0";
    }



    QString query="insert into anagrafica(ragione_sociale,indirizzo,citta,cap,provinciaZAP,nazione,tel,contatto,cliente,fornitore,trasportatore,note,stampatore) values('"+SRagSoc+"','"+sIndirizzo+"','"+sCity+"','"+sCap+"','"+sProv+"','"+sNaz+"','"+sTel+"','"+ui->leContatto->text()+"','"+sCliente+"','"+sFornitore+"','"+sTrasportatore+"','"+sNote+"','"+sStampatore+"')";

    if(QMessageBox::question(this,QApplication::applicationName(),"Salvare la entry di anagrafica?",QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
    {
      bool b=q.exec(query);
      qDebug()<<q.lastQuery()<<q.lastError();

      if (!b)
      {
         QMessageBox::information(this,QApplication::applicationName(),"Errore salvando la entry\n"+q.lastError().text(),QMessageBox::Ok);

      }
      else
      {

         QMessageBox::information(this,QApplication::applicationName(),"Modifiche salvate",QMessageBox::Ok);
         emit done();
      }



    }
}

void HNewAnagrafica::on_pushButton_2_clicked()
{
    if(QMessageBox::Ok==QMessageBox::question(this,QApplication::applicationName(),"Chiudere?",QMessageBox::Ok|QMessageBox::Cancel))
   {

        this->close();
   }
}
