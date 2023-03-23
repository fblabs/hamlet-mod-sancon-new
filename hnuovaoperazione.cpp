#include "hnuovaoperazione.h"
#include "ui_hnuovaoperazione.h"
#include <QSqlDatabase>
#include <QSqltableModel>
#include <QSqlRelationalTableModel>
#include <hreadonlymodelnew.h>
#include <QSqlRelationalDelegate>
#include <QSqlQuery>
#include <QDate>
#include <QMessageBox>
#include <QCompleter>
#include "huser.h"
#include <QDebug>
#include <QSqlError>

HnuovaOperazione::HnuovaOperazione(HUser *puser,QSqlDatabase pdb,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HnuovaOperazione)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    ui->setupUi(this);
    user=puser;
    db=pdb;


    tbm = new HReadOnlyModelNew(0,db);
    tbm->setTable("operazioni");
    tbm->setRelation(1,QSqlRelation("lotdef","ID","lot"));
    tbm->setRelation(3,QSqlRelation("utenti","ID","nome"));
    tbm->setRelation(4,QSqlRelation("prodotti","ID","descrizione"));
    tbm->setRelation(5,QSqlRelation("azioni","ID","descrizione"));
    tbm->setRelation(7,QSqlRelation("unita_di_misura","ID","descrizione"));

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

   // ui->tableView->setItemDelegate(new QSqlRelationalDelegate(tbm));
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnHidden(3,true);
    ui->tableView->setModel(tbm);


    tbm->setSort(0,Qt::DescendingOrder);
    tbm->setFilter("operazioni.data >=DATE(CURDATE())");
    tbm->select();

    tbm->setHeaderData(0,Qt::Horizontal,QObject::tr("ID"));
    tbm->setHeaderData(1,Qt::Horizontal,QObject::tr("Lotto"));// 1 lotdef.lot
    tbm->setHeaderData(2,Qt::Horizontal,QObject::tr("Data"));
    tbm->setHeaderData(3,Qt::Horizontal,QObject::tr("Operatore"));//3 utenti.ID
    tbm->setHeaderData(4,Qt::Horizontal,QObject::tr("Prodotto"));//4 prodotti.ID
    tbm->setHeaderData(5,Qt::Horizontal,QObject::tr("Azione"));//5 azioni.ID
    tbm->setHeaderData(6,Qt::Horizontal,QObject::tr("Quantità"));
    tbm->setHeaderData(7,Qt::Horizontal,QObject::tr("Unità di misura"));
    tbm->setHeaderData(8,Qt::Horizontal,QObject::tr("Note"));




    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnHidden(3,true);

    ui->cbShowPackages->setVisible(false);


    ui->deScadenza->setDate(QDate::currentDate().addYears(2));

    listaFornitori=new QSqlTableModel(0,db);
    listaProdotti=new QSqlTableModel(0,db);
    listaTipologie=new QSqlTableModel(0,db);
    listaUnitaDiMisura=new QSqlTableModel(0,db);
    listaTipilot=new QSqlTableModel(0,db);


    listaFornitori->setTable("anagrafica");
    listaFornitori->setFilter("fornitore=1");
    listaFornitori->setSort(1,Qt::AscendingOrder);

    listaProdotti->setTable("prodotti");
    listaProdotti->setSort(1,Qt::AscendingOrder);
    listaTipologie->setTable("tipi_prodotto");
    listaTipologie->setSort(1,Qt::AscendingOrder);
    listaUnitaDiMisura->setTable("unita_di_misura");
    listaUnitaDiMisura->setSort(1,Qt::AscendingOrder);
    listaTipilot->setTable("tipi_lot");
    listaTipilot->setSort(1,Qt::AscendingOrder);

    listaFornitori->select();


    listaTipologie->select();
    listaUnitaDiMisura->select();
    listaTipilot->select();

    lots=new QSqlTableModel(0,db);

    lots->setTable("lotdef");
    lots->select();


    basefilter="attivo=2 and year(data)>year(data)-3";
    lots->setFilter(basefilter);

    ui->cbtipo->setModel(listaTipologie);
    ui->cbtipo->setModelColumn(1);

    ui->cbAnagrafica->setModel(listaFornitori);
     ui->cbAnagrafica->setModelColumn(1);


     ui->lvProdotti->setModel(listaProdotti);
     ui->lvProdotti->setModelColumn(1);
     listaProdotti->select();

    ui->cbTipoLot->setModel(listaTipilot);
    ui->cbTipoLot->setModelColumn(1);



    ui->cbUM->setModel(listaUnitaDiMisura);
    ui->cbUM->setModelColumn(1);


    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->lvProdotti->setEnabled(false);
    ui->leProdotti->setEnabled(false);
    ui->cbtipo->setEnabled(false);
    ui->cbtipo->setEnabled(false);
    ui->cbAnagrafica->setEnabled(false);
    ui->cbtipo->setCurrentIndex(2);


    QCompleter *comforn=new QCompleter(listaFornitori);
    comforn->setCaseSensitivity(Qt::CaseInsensitive);
    comforn->setCompletionColumn(1);
    comforn->setCompletionMode(QCompleter::PopupCompletion);

    ui->cbAnagrafica->setCompleter(comforn);


    connect(ui->lvProdotti->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this, SLOT(setProdottoText()));


    setUiforCarico();



    QApplication::setOverrideCursor(Qt::ArrowCursor);

}

HnuovaOperazione::~HnuovaOperazione()
{
    delete ui;

}

void HnuovaOperazione::setFilterProdotti()
{
  //  QString f;
  //  f="ID in (SELECT ID_prodotto from ricette,associazioni where ricette.ID = associazioni.ID_ricetta and associazioni.ID_cliente="+listaFornitori->index(ui->cbAnagrafica->currentIndex(),0).data(0).toString();
  //  listaProdotti->setFilter(f);

}




void HnuovaOperazione::setUiforCarico()
{


    ui->label->setText("Fornitore:");


    ui->cbtipo->setVisible(true);
    ui->cbUM->setVisible(true);
    ui->label_2->setVisible(true);
    ui->cbtipo->setVisible(true);
    ui->label_10->setVisible(false);
    ui->leLotto->setVisible(false);
    ui->label_5->setVisible(true);
    ui->cbUM->setVisible(true);
    ui->label_7->setVisible(true);
    ui->cbScadenza->setVisible(true);
    ui->deScadenza->setVisible(true);
    ui->cbScadenza->setChecked(false);


    ui->label_9->setVisible(true);
    ui->leNewLot->setVisible(true);
    ui->label_6->setVisible(true);
    ui->leLotfornitore->setVisible(true);


    ui->cbTipoLot->setVisible(true);
    ui->label_12->setVisible(true);
    ui->label_11->setVisible(true);
    ui->leProdottoCreato->setVisible(true);
    ui->leProdotti->setVisible(true);

    ui->label_14->setVisible(true);
    ui->label_13->setVisible(true);
    ui->label_8->setVisible(true);
    ui->cbScadenza->setVisible(true);
    ui->ledispLottoforn->setVisible(true);
    ui->leLotfornitore->setVisible(true);
    ui->leScadenza->setVisible(true);

    listaProdotti->setFilter("");
    listaProdotti->select();

    ui->lvProdotti->setModel(listaProdotti);
    ui->lvProdotti->setModelColumn(1);


 //   ui->cbtipo->setCurrentIndex(2);
    disconnect(ui->lvProdotti->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this, SLOT(setLotsFilter()));
 //   disconnect(ui->cbAnagrafica,SIGNAL(currentIndexChanged(int)),this,SLOT(setAnagFilter()));





}

void HnuovaOperazione::setUiForScarico()
{



    // ui->label->setVisible(false);
    ui->label->setText("Destinazione:");
   // ui->cbAnagrafica->setVisible(false);
   // ui->lvProdotti->setVisible(false);
   // ui->lvProdotti->setEnabled(true);
   // listaProdotti->select();
    ui->cbtipo->setVisible(false);
    ui->cbUM->setVisible(true);
    ui->label_2->setVisible(false);
    ui->cbtipo->setVisible(false);
    ui->label_10->setVisible(true);
    ui->leLotto->setVisible(true);
    ui->leLotto->clear();
    ui->label_5->setVisible(false);
    ui->cbUM->setVisible(false);
    ui->label_7->setVisible(false);
    ui->deScadenza->setVisible(false);
    ui->label_9->setVisible(false);
    ui->leNewLot->setVisible(false);
    ui->label_6->setVisible(false);
    ui->leLotfornitore->setVisible(false);
    ui->label_4->setVisible(true);
    ui->tNote->setVisible(true);
    ui->cbTipoLot->setVisible(false);
    ui->label_12->setVisible(false);
    ui->leProdotti->setVisible(false);
    ui->label_11->setVisible(false);
    ui->leProdottoCreato->setVisible(false);

   // ui->cbtipo->setCurrentIndex(2);
    ui->label_14->setVisible(false);
    ui->label_13->setVisible(false);
    ui->label_8->setVisible(false);
    ui->cbScadenza->setVisible(false);
    ui->ledispLottoforn->setVisible(false);
    ui->leLotfornitore->setVisible(false);
    ui->leScadenza->setVisible(false);

    ui->leQuantita->clear();
    ui->leLotto->clear();
//    setListaLots();
    setLotsFilter();



    listaFornitori->setFilter("");

    connect(ui->lvProdotti->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this, SLOT(setProdottoText()));
    connect(ui->lvProdotti->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this, SLOT(setLotsFilter()));
  //  connect(ui->cbAnagrafica,SIGNAL(currentIndexChanged(int)),this,SLOT(setAnagFilter()));






}



void HnuovaOperazione::setProdottoText()
{
    QString prodotto;
    prodotto=ui->lvProdotti->model()->index(ui->lvProdotti->currentIndex().row(),1).data(0).toString();

    ui->leProdottoCreato->setText(prodotto);
}

void HnuovaOperazione::setLotsFilter()
{

   QString prodotto;
   QString prfilt;
   QString flt;

    prodotto=ui->lvProdotti->model()->index(ui->lvProdotti->currentIndex().row(),0).data(0).toString();




   if(ui->cbShowPackages->isChecked())
   {
       flt=basefilter + " and tipo=4";
   }
   else
   {
       flt=basefilter;
   }


   lots->select();


   lots->setSort(3,Qt::DescendingOrder);
   QCompleter *com = new QCompleter(lots);
   com->setCompletionColumn(1);
   com->setCaseSensitivity(Qt::CaseInsensitive);
   com->setCompletionMode(QCompleter::PopupCompletion);
   ui->leLotto->setCompleter(com);

   if(ui->radioButton_2->isChecked())
   {

    prodotto=ui->lvProdotti->model()->index(ui->lvProdotti->currentIndex().row(),0).data(0).toString();
    prfilt=" and prodotto="+prodotto;
    lots->setFilter(flt + prfilt);
   }
   else
   {

       QSqlTableModel *lots=static_cast<QSqlTableModel*>(ui->leLotto->completer()->model());
       lots->setFilter(flt);
   }



  // // qDebug()<<"setLotsFilter: "<<lots->filter();



}

void HnuovaOperazione::on_pushButton_4_clicked()
{
    emit trigger();
    this->deleteLater();
}

bool HnuovaOperazione::saveNewLot(QString nl)
{
    bool b;
    QSqlQuery q(db);
    QString data,scadenza,lotforn;
    int idprod;
    int tipo;
    double giacenza;
    int anagrafica;
    int attivo=2;
    QString note=ui->tNote->toPlainText();


    idprod=ui->lvProdotti->model()->index(ui->lvProdotti->currentIndex().row(),0).data(0).toInt();

    data=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    tipo=ui->cbTipoLot->model()->index(ui->cbTipoLot->currentIndex(),0).data(0).toInt();
    giacenza=ui->leQuantita->text().toDouble();


     if(ui->cbScadenza->isChecked())
    {
       scadenza=QVariant(QVariant::String).toString();
    }
    else
    {
       scadenza=ui->deScadenza->date().toString("yyyy-MM-dd");
    }

    anagrafica=ui->cbAnagrafica->model()->index(ui->cbAnagrafica->currentIndex(),0).data(0).toInt();
    lotforn=ui->leLotfornitore->text();

    QString sql="INSERT INTO lotdef(lot,prodotto,data,giacenza,um,scadenza,anagrafica,lot_fornitore,tipo,attivo,note) values (:lot,:prodotto,:data,:giacenza,:um,:scadenza,:anagrafica,:lot_fornitore,:tipo,:attivo,:note)";
    q.prepare(sql);
    q.bindValue(":lot",QVariant(nl));
    q.bindValue(":prodotto",QVariant(idprod));
    q.bindValue(":data",QVariant(data));
    q.bindValue(":giacenza",QVariant(giacenza));
    q.bindValue(":um",QVariant(ui->cbUM->model()->index(ui->cbUM->currentIndex(),0).data(0)));
    q.bindValue(":scadenza",QVariant(scadenza));
    q.bindValue(":anagrafica",QVariant(anagrafica));
    q.bindValue(":lot_fornitore",QVariant(lotforn));
    q.bindValue(":tipo",QVariant(tipo));
    q.bindValue(":attivo",QVariant(attivo));
    q.bindValue(":note",QVariant(note));

   // // qDebug()<<q.boundValue(3).toString();

    b=q.exec();

    if (b)
    {

        ui->leNewLot->setText(nl);
        ui->ledispLottoforn->setText(lotforn);
        ui->leScadenza->setText(ui->deScadenza->date().toString("dd-MM-yyyy"));
// // qDebug()<<q.lastQuery();
        q.clear();
    }
    else
    {

       QMessageBox::warning(this,QApplication::applicationName(),"Errore salvando il nuovo lotto\n"+q.lastError().text()+"\n"+q.lastQuery(),QMessageBox::Ok);

    }
    return b;
}

bool HnuovaOperazione::saveOperation(bool isCarico)
{
    bool b;
    db.transaction();

    if(isCarico)
    {
        b=saveOperationCarico();

    }
    else
    {
        b=saveOperationScarico();
    }

    if(b)
    {
        qDebug()<<"seveOperaton: niente da segnalare";
        db.commit();


        tbm->select();


    }
    else
    {
        db.rollback();
    }


   return b;

}


bool HnuovaOperazione::saveOperationScarico()
{
    QSqlQuery q(db);
    bool b;

    QString lot=ui->leLotto->text();

    int idlot;
    int lum;
    QSqlQuery l(db);
    QString sql="SELECT ID,um from lotdef where lot=:lot";
    l.prepare(sql);
    l.bindValue(":lot",QVariant(lot));
    l.exec();
    l.first();
    idlot=l.value(0).toInt();
    lum=l.value(1).toInt();

    QString data=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    QStringList prodottolist =ui->leLotto->text().split("-");

    int prodotto=prodottolist.at(1).toInt();
    int azione=2;
    double quantita=ui->leQuantita->text().toDouble();
    //int UM=ui->cbUM->model()->index(ui->cbUM->currentIndex(),0).data(0).toInt();
    QString note="";

    QString query="INSERT INTO operazioni(IDlotto,data,utente,IDprodotto,azione,quantita,um,note) VALUES (:lot,:data,:utente,:prodotto,:azione,:quantita,:um,:note)";

    q.prepare(query);
    q.bindValue(":lot",QVariant(idlot));
    q.bindValue(":data",QVariant(data));
    q.bindValue(":utente",QVariant(user->getID()));
    q.bindValue(":prodotto",QVariant(prodotto));
    q.bindValue(":azione",QVariant(azione));
    q.bindValue(":quantita",QVariant(quantita));
    q.bindValue(":um",QVariant(lum));
    q.bindValue(":note",QVariant(note));


    b=q.exec();
    if (b)
    {

      tbm->select();


    }


    return b;
}


bool HnuovaOperazione::saveOperationCarico()
{
  //ID=-1,lot=createID,data=now(),utente=user,IDprodotto=listaprodotti,azione=1,quantita,um,note
    bool b;

    int idprodotto=ui->lvProdotti->model()->index(ui->lvProdotti->currentIndex().row(),0).data(0).toInt();
    QString nuovolot;


    QSqlQuery q(db);
    QString query ="SELECT createID(:id)";
    q.prepare(query);
    q.bindValue(":id",idprodotto);
    q.exec();
    q.first();
    nuovolot=q.value(0).toString();


    b=saveNewLot(nuovolot);



    if (!b)
    {
       QMessageBox::information(this,"DEBUG","newlot: "+nuovolot+"salvataggio fallito",QMessageBox::Ok);
       return b;

    }



//salvo l'operazione
    int idlotto=lastInsertID();
    QString data=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    int prodotto=ui->lvProdotti->model()->index(ui->lvProdotti->currentIndex().row(),0).data(0).toInt();
    int azione=1;
    double quantita=ui->leQuantita->text().toDouble();
    int UM=ui->cbUM->model()->index(ui->cbUM->currentIndex(),0).data(0).toInt();
    QString note=ui->tNote->toPlainText();


    QString query2="INSERT INTO operazioni(IDlotto,data,utente,IDprodotto,azione,quantita,um,note) VALUES (:idlotto,:data,:utente,:prodotto,:azione,:quantita,:um,:note)";
    QSqlQuery op(db);

    op.prepare(query2);
    op.bindValue(":idlotto",QVariant(idlotto));
    op.bindValue(":data",QVariant(data));
    op.bindValue(":utente",QVariant(user->getID()));
    op.bindValue(":prodotto",QVariant(prodotto));
    op.bindValue(":azione",QVariant(azione));
    op.bindValue(":quantita",QVariant(quantita));
    op.bindValue(":um",QVariant(UM));
    op.bindValue(":note",QVariant(note));

    b=op.exec();


    if(b)
    {

        ui->leQuantita->setText("");
        ui->leLotfornitore->setText("");
        ui->tNote->clear();

       // op.clear();

    }else{
        QMessageBox::information(this,"DEBUG",op.lastError().text(),QMessageBox::Ok);
    }

    double quantitaope=0.0;
    query2="SELECT getgiacenza(:id)";
    op.prepare(query2);
    op.bindValue(":id",QVariant(idlotto));
    b=op.exec();
    if(b)
    {
      op.first();
      quantitaope=op.value(0).toDouble();
    }



    ui->leQuantita->setText(QString::number(quantitaope,'f',4));

    return b;

}

int HnuovaOperazione::lastInsertID()
{
    QSqlQuery q(db);
    QString sql="SELECT last_insert_id();";
    q.prepare(sql);
    q.exec();

    q.first();

    return q.value(0).toInt();
}



QString HnuovaOperazione::getNewLot(QString prodotto)
{
    QString newLot;
    QSqlQuery nl(db);

    nl.exec("SELECT createID("+ prodotto +")");
    nl.next();
    newLot=nl.value(0).toString();
//// qDebug()<<nl.lastQuery();
    return newLot;
}

void HnuovaOperazione::on_pushButton_clicked()
{
   bool b;


   b=saveOperation(ui->radioButton->isChecked());

   if (b)
   {



        ui->pushButton->setEnabled(false);
        ui->cbScadenza->setChecked(false);
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(true);
        ui->tNote->clear();
       // ui->leLotto->clear();
        ui->leQuantita->clear();


      //  ui->radioButton->setEnabled(false);
      //  ui->radioButton_2->setEnabled(false);
        ui->lvProdotti->setEnabled(false);
        ui->leProdotti->setEnabled(false);
        ui->cbtipo->setEnabled(false);
        ui->cbAnagrafica->setEnabled(false);



        emit trigger();
        QMessageBox::information(this,QApplication::applicationName(),"Operazione salvata",QMessageBox::Ok);



   }
   else
   {
       db.rollback();
       QMessageBox::warning(this,QApplication::applicationName(),"Errore!",QMessageBox::Ok);

   }







}


void HnuovaOperazione::on_radioButton_2_toggled(bool checked)
{
    if (checked)
    {
        setUiForScarico();
    }
}

void HnuovaOperazione::on_radioButton_toggled(bool checked)
{
    if(checked)
    {
        setUiforCarico();
    }
}


void HnuovaOperazione::on_cbScadenza_toggled(bool checked)
{
    ui->deScadenza->setVisible(!checked);
}


void HnuovaOperazione::on_leProdotti_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    QString filter;
    filter="descrizione LIKE '%";
    filter.append(ui->leProdotti->text());
    filter.append("%'");
}

void HnuovaOperazione::on_leLotto_textChanged(const QString &arg1)
{
    QString filter;
    filter="lot LIKE '%";
    filter.append(arg1);
    filter.append("%'");

   // // qDebug()<<listaProdotti->lastError().text()<<filter;
    setLotsFilter();
}

void HnuovaOperazione::on_pushButton_2_clicked()
{
    ui->leQuantita->setText("");
    ui->leLotfornitore->setText("");
    ui->tNote->clear();
    ui->cbScadenza->setChecked(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(true);


  //  ui->radioButton->setEnabled(false);
  //  ui->radioButton_2->setEnabled(false);
    ui->lvProdotti->setEnabled(false);
    ui->leProdotti->setEnabled(false);
    ui->cbtipo->setEnabled(false);
    ui->cbAnagrafica->setEnabled(false);
    ui->leQuantita->clear();
    ui->leLotto->clear();
}

void HnuovaOperazione::on_pushButton_3_clicked()
{
   // if(QMessageBox::question(this,QApplication::applicationName(),"Nuova operazione:\nVerificare di avere salvato i dati dell'eventuale operazione precedente!\nProcedere?",QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
  //  {
        ui->leQuantita->setText("");
        ui->leLotfornitore->setText("");
        ui->tNote->clear();
        ui->cbScadenza->setChecked(false);
  //  }
    ui->pushButton_3->setEnabled(false);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);

  //  ui->radioButton->setEnabled(true);
  //  ui->radioButton_2->setEnabled(true);
    ui->lvProdotti->setEnabled(true);
    ui->leProdotti->setEnabled(true);
    ui->cbtipo->setEnabled(true);
    ui->cbAnagrafica->setEnabled(true);

    if(ui->radioButton->isChecked())
    {
        setUiforCarico();
    }else{
        setUiForScarico();
        setLotsFilter();
    }



  //  QSqlTableModel* mod=static_cast<QSqlTableModel*>(ui->leLotto->completer()->model());
 //   mod->select();
}







void HnuovaOperazione::on_cbShowPackages_toggled(bool checked)
{
       Q_UNUSED(checked);
       setLotsFilter();



}

void HnuovaOperazione::on_cbtipo_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    QString tipo;
    tipo=ui->cbtipo->currentText();
   // // qDebug()<<"tipo:"+tipo;
    QSqlQuery q(db);
    QString sql="SELECT ID from tipi_prodotto where descrizione=:tipo";
    q.prepare(sql);
    q.bindValue(0,QVariant(tipo));
    q.exec();
    q.first();
    tipo=q.value(0).toString();


   QString filter = "tipo=" + tipo;


    listaProdotti->setFilter(filter);
   // // qDebug()<<"cbtipo->ixc"<<listaProdotti->filter()<<listaProdotti->query().lastQuery();
}
