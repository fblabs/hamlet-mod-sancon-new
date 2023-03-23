#include "hproduction.h"
#include "ui_hproduction.h"

#include <QSqlDatabase>
#include <QSqlQuery>

// #include <QDebug>
#include <QSqlError>
#include <QDateTime>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QSqlRecord>
#include <QCompleter>
#include "hprint.h"
#include <QTextTable>
#include <QSqlQueryModel>
#include "haddlotinproduction.h"
#include <math.h>
#include <QDebug>
#include <QStandardItemModel>
#include "hdatatopass.h"
#include "hlastlots.h"
#include "huser.h"
#include <QSettings>

enum ACTION{
    RESET=0,
    WAIT=1
};

HProduction::HProduction(HUser *puser,QSqlDatabase pdb,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HProduction)
{
    ui->setupUi(this);

    user=puser;
    db=pdb;
    qDebug()<<"GETPRODUZIONE_U"<<QString::number(user->get_produzione_u()>0);

    ui->pushButton_5->setEnabled(user->get_produzione_u()>0);
    ui->pushButton_2->setEnabled(user->get_produzione_u()>0);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_7->setEnabled(user->get_produzione_u()>0);
    ui->pushButton_11->setEnabled(user->get_produzione_u()>0);



    QSettings settings("hamletmod.ini",QSettings::IniFormat);
    preferred_db=settings.value("preferred_lots").toString();



     prefdb=QSqlDatabase::addDatabase("QSQLITE");
     prefdb.setDatabaseName(preferred_db);
     prefdb.open();

    ui->dateEdit->setVisible(false);
    ui->label_9->setVisible(false);
    ui->pushButton_10->setEnabled(false);
    ui->pushButton->setVisible(false);
    ui->checkBox->setVisible(false);

    modifyLot=false;

    tmTipiLotti=new QSqlTableModel(0,db);
    tmTipiLotti->setTable("tipi_lot");
    tmTipiLotti->select();
    tmTipiLotti->setFilter("ID=3");
    tmTipiLotti->setSort(1,Qt::AscendingOrder);

    ui->cbTipoLotto->setModel(tmTipiLotti);
    ui->cbTipoLotto->setModelColumn(1);
    ui->cbTipoLotto->setEnabled(true);

    tmUm=new QSqlTableModel(0,db);
    tmUm->setTable("unita_di_misura");
    tmUm->select();
    tmUm->setSort(1,Qt::AscendingOrder);

    ui->cbUm->setModel(tmUm);
    ui->cbUm->setModelColumn(1);



    getClients();
    ui->cbClienti->setCurrentIndex(ui->cbClienti->model()->rowCount());
    ui->cbClienti->setCurrentIndex(0);

    ui->lvRicette->setEnabled(true);
    ui->lvSubclienti->setVisible(false);

  /*  ui->leQtyTotal->setEnabled(true);
    ui->pushButton_5->setVisible(true);
    ui->pushButton_6->setVisible(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    ui->leNuovoLot->setText("");
    ui->label_6->setVisible(true);*/
    ui->dateEdit->setDate(QDate::currentDate().addYears(2));

    setAddProductFuoriRicettaUI(false);

}


HProduction::~HProduction()
{
    delete ui;
}
void HProduction::getSubclients()
{
    QSqlQueryModel *qm=new QSqlQueryModel();
    int cliente=ui->cbClienti->model()->index(ui->cbClienti->currentIndex(),0).data(0).toInt();
    QSqlQuery q(db);
    QString sql="SELECT ID,ragione_sociale from anagrafica where IDCliente=:id";

    q.prepare(sql);
    q.bindValue(":id",QVariant(cliente));
    q.exec();
    qm->setQuery(q);
    // qDebug()<<q.lastError().text()<<QString::number(qm->rowCount());
    ui->lvSubclienti->setModel(qm);
    ui->lvSubclienti->setModelColumn(1);
    if (qm->rowCount()>0)
    {


        connect(ui->lvSubclienti->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(getRecipesForClient()));

    }
    else
    {

        disconnect(ui->lvSubclienti->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(getRecipesForClient()));

    }



}


void HProduction::getLotModel()
{
    QSqlTableModel *lm=new QSqlTableModel(0,db);
    lm->setTable("lotdef");
    //  QCompleter *comp=new QCompleter(lm);
    //  comp->setCompletionMode(QCompleter::PopupCompletion);
    //  comp->setCompletionColumn(1);
    lm->setSort(3,Qt::DescendingOrder);
    lm->select();
    //   ui->leLotToEdit->setCompleter(comp);

}

void HProduction::recalculateTotal()
{
    QStandardItemModel *model=static_cast<QStandardItemModel*>(ui->tableView->model());
    double quantita=0.0;

    for (int x=0;x<model->rowCount();x++)
    {
        quantita += model->index(x,5).data(0).toDouble();

    }

    QString qua=QString::number(quantita,'f',3);
    //   ui->leQtyTotal->setText(qua);



}

void HProduction::calculateActualTotal()
{
    QStandardItemModel *model=static_cast<QStandardItemModel*>(ui->tableView->model());
    double quantita=0.0;

    for (int x=0;x<model->rowCount();x++)
    {
        bool ok;
        quantita += model->index(x,5).data(0).toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this,QApplication::applicationName(),"Errore nella quantità",QMessageBox::Ok);
            return;
        }
        // // qDebug()<<"calculateactualtotalrow"<<x<<QString::number(quantita);

    }
    QString qta =QString::number(quantita,'f',3);


    //ui->leQtyTotal->setText(qta);
    // ui->leQuaRic->setText(QString::number(quantita,'f',3));



}

void HProduction::updateTotals()
{
    bool ok;

    double dafare = ui->leQtyTotal->text().toDouble(&ok);
    if(!ok)
    {
        // // qDebug()<<"updatetotals"<<QString::number(dafare,'f',6);
    }
    // ui->leQuaRic->setText(QString::number(dafare,'f',3));
    double sommarighe=0;
    double factor=0;
    double result=0;
    QStandardItemModel* model=static_cast<QStandardItemModel*>(ui->tableView->model());

    for (int x=0;x<model->rowCount();x++)
    {
        sommarighe += model->index(x,2).data(0).toDouble();

    }


    factor = dafare / sommarighe;

    // // // qDebug()<<"updatetotals"<<QString::number(sommarighe,'f',6);

    for (int j=0;j<model->rowCount();j++)
    {
        QModelIndex i = model->index(j,2);
        QModelIndex m = model->index(j,5);
        result =i.data().toDouble()* factor;
        QString resulttoadd=QString::number(result,'f',3);
        model->setData(m,QVariant(resulttoadd));

    }





}

void HProduction::getClients()
{
    QSqlQuery q(db);
    qmClienti=new QSqlQueryModel();

    QString sql="SELECT ID,ragione_sociale from anagrafica where (cliente=1 or subcliente=1) and visibile>0 order by ragione_sociale ASC";

    q.exec(sql);

    qmClienti->setQuery(q);

    qDebug()<<q.lastError().text();

    ui->cbClienti->setModel(qmClienti);
    ui->cbClienti->setModelColumn(1);

    connect(ui->cbClienti,SIGNAL(currentIndexChanged(int)),this,SLOT(getRecipesForClient()));
    ui->cbClienti->setCurrentIndex(0);
    QCompleter *cmp=new QCompleter(qmClienti);
    cmp->setCompletionColumn(1);
    cmp->setCompletionMode(QCompleter::PopupCompletion);
    cmp->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cbClienti->setCompleter(cmp);
}
void HProduction::getLotToModify(QString lot)
{

    ui->tableView->setEnabled(true);
    ui->lvRicette->setEnabled(false);

    ui->pushButton_5->setVisible(false);
    ui->pushButton_6->setVisible(user->get_produzione_u()>0);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(user->get_produzione_u()>0);
    ui->pushButton_7->setEnabled(user->get_produzione_u()>0);
    //ui->label->setVisible(true);
    //ui->cbQuanti->setVisible(true);
    //ui->lvLastLots->setVisible(true);
    ui->leQtyTotal->setReadOnly(user->get_produzione_u()>0);
    ui->leQtyTotal->setEnabled(user->get_produzione_u()>0);
    ui->cbTipoLotto->setEnabled(user->get_produzione_u()>0);


    ui->tableView->horizontalHeader()->resizeSections(QHeaderView::Stretch);

    double quantitatotale;
    QStandardItemModel *model;
    // QString lot;
    //ricavo lo id del lotto
    QSqlQuery k(db);

    QString sqlot="SELECT ID from lotdef where lot=:lot";
    k.prepare(sqlot);
    k.bindValue(":lot",QVariant(lot));
    k.exec();
    k.first();
    lottomodifica=k.value(0).toInt();
    // qDebug()<<"idlotto="<<QString::number(idlotto);


    //QString sql="SELECT operazioni.IDProdotto,prodotti.descrizione,operazioni.quantita,lotdef.ID,lotdef.lot from operazioni,prodotti,lotdef  where operazioni.IDprodotto=prodotti.ID and lotdef.ID=operazioni.IDlotto and operazioni.ID in (select operazione from composizione_lot where ID_lotto=:idlotto)";
    QString sql="SELECT operazioni.IDProdotto,prodotti.descrizione,operazioni.quantita,lotdef.ID,lotdef.lot,operazioni.ID from operazioni,prodotti,lotdef where operazioni.IDprodotto=prodotti.ID and operazioni.ID and lotdef.ID=operazioni.IDlotto and operazioni.ID in (select operazione from composizione_lot where ID_lotto=:idlotto)";
    k.prepare(sql);
    k.bindValue(":idlotto",QVariant(lottomodifica));
    k.exec();
    // qDebug()<< k.lastError().text()<<k.lastQuery();

    model = new QStandardItemModel(0,7);

    model->setHeaderData(0,Qt::Horizontal,"ID prodotto",0);
    model->setHeaderData(1,Qt::Horizontal,"Prodotto",0);
    model->setHeaderData(2,Qt::Horizontal,"Quantità",0);
    model->setHeaderData(3,Qt::Horizontal,"ID lotto",0);
    model->setHeaderData(4,Qt::Horizontal,"Lotto",0);
    model->setHeaderData(5,Qt::Horizontal,"Quantità",0);


    quantitatotale=0.0;

    //  q.first();
    while(k.next())
    {

        quantitatotale +=k.value(2).toDouble();
        //  model->appendRow(createRecipeRow(q.value(1).toString(),q.value(2).toString(),QString::number(q.value(3).toDouble(),'f',2),"","pippo"));
        QList<QStandardItem*> columns;
        //   QStandardItem* IDriga=new QStandardItem(q.value(0).toString());

        QStandardItem* ID_prodotto=new QStandardItem(k.value(0).toString());
        QStandardItem* prodotto=new QStandardItem(k.value(1).toString());
        QStandardItem* quantita=new QStandardItem(QString::number(k.value(2).toDouble(),'f',4));
        QStandardItem* idlotto=new QStandardItem(k.value(3).toString());
        QStandardItem* lotto=new QStandardItem(k.value(4).toString());
        QStandardItem* quanteff=new QStandardItem(k.value(2).toString());
        QStandardItem* IDriga=new QStandardItem (k.value(5).toString());


        //columns.append(IDLotto);

        columns.append(ID_prodotto);
        columns.append(prodotto);
        columns.append(quantita);
        columns.append(idlotto);
        columns.append(lotto);
        columns.append(quanteff);
        columns.append(IDriga);

        model->appendRow(columns);

    }
    QString qta=QString::number(quantitatotale,'f',3);

    //   ui->leQtyTotal->setText(qta);

    // qmrighe->setQuery(q);
    ui->tableView->setModel(model);
    //ui->tableView->horizontalHeader()->setStretchLastSection(true);
    // ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // connect(ui->lvRicette->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(updateTotals()));                                                                                                 );
    connect(ui->tableView->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(productSelected()));
    connect(ui->tableView->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(updateTotals()));


}

void HProduction::getRecipesForClient()
{
    int idcliente;
    if (ui->checkBox->isChecked())
    {
        idcliente=ui->lvSubclienti->model()->index(ui->lvSubclienti->currentIndex().row(),0).data(0).toInt();
    }
    else
    {
        idcliente=ui->cbClienti->model()->index(ui->cbClienti->currentIndex(),0).data(0).toInt();
    }

    ui->tableView->setModel(0);


    //   QString qs="SELECT select prodotti.ID,ricette.ID,prodotti.descrizione from ricette,associazioni,prodotti,anagrafica where prodotti.ID=ricette.ID_prodotto and ricette.ID=associazioni.ID_ricetta and associazioni.ID_cliente=anagrafica.ID and associazioni.ID_cliente="+idcliente;
    QString qs="select ricette.ID,prodotti.ID,prodotti.descrizione from prodotti, ricette, associazioni where ricette.ID=associazioni.ID_ricetta and prodotti.ID=ricette.ID_prodotto and associazioni.visualizza=1 and associazioni.visualizza=1 and associazioni.ID_cliente=:idcliente";
    QSqlQuery q(db);
    q.prepare(qs);
    q.bindValue(":idcliente",QVariant(idcliente));

    qmRicette=new QSqlQueryModel();

    q.exec();

    // qDebug()<<q.lastError().text();

    qmRicette->setQuery(q);

    ui->lvRicette->setModel(qmRicette);
    ui->lvRicette->setModelColumn(2);

    
    connect(ui->lvRicette->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(getRecipe()));


    ui->lvRicette->setCurrentIndex(ui->lvRicette->model()->index(0,0));
    ui->lvRicette->selectionModel()->select(ui->lvRicette->model()->index(0,0),QItemSelectionModel::Select);
    ui->lvRicette->setFocus();



}

void HProduction::getRecipe()
{


    //selezionato un prodotto ne prendo la ricetta


    int idricetta=ui->lvRicette->model()->index(ui->lvRicette->currentIndex().row(),0).data(0).toInt();



    ui->tableView->horizontalHeader()->resizeSections(QHeaderView::Stretch);

    //note

    QSqlQuery n(db);
    QString sql2;
    sql2="SELECT ricette.note from prodotti,ricette where prodotti.ID=ricette.ID_prodotto and ricette.ID=:idricetta";
    n.prepare(sql2);
    n.bindValue(":idricetta",QVariant(idricetta));
    n.exec();
    n.first();
    ui->textBrowser->setText(n.value(0).toString());
    ui->lbRicetta->setText(ui->lvRicette->model()->index(ui->lvRicette->currentIndex().row(),2).data(0).toString());


    //--------------------




    //QString idprodotto,descrizione,quantita,lotto,quantitaeffettivadaaggiungere;




    QString sql="select distinct prodotti.ID ,prodotti.descrizione,prodotti.allergenico,righe_ricette.quantita from prodotti,righe_ricette where righe_ricette.ID_prodotto=prodotti.ID and righe_ricette.ID_ricetta=:idricetta order by righe_ricette.quantita desc";
    //writeRed=new QList<int>();
    QSqlQuery q(db);
    qmod=new QSqlQueryModel();


    q.prepare(sql);
    q.bindValue(":idricetta",QVariant(idricetta));

    q.exec();
    qmod->setQuery(q);




    model = new QStandardItemModel(0,7);

    model->setHeaderData(0,Qt::Horizontal,"ID Prodotto",0);
    model->setHeaderData(1,Qt::Horizontal,"Prodotto",0);
    model->setHeaderData(2,Qt::Horizontal,"Quantità ricetta",0);
    model->setHeaderData(3,Qt::Horizontal,"ID Lotto",0);
    model->setHeaderData(4,Qt::Horizontal,"Lotto",0);
    model->setHeaderData(5,Qt::Horizontal,"Quantità effettiva",0);
    model->setHeaderData(6,Qt::Horizontal,"Allergene",0);

    double quantitatot=0.0;


    for(int row=0; row<qmod->rowCount();row++)
    {

        bool ok;

        // alle=q.value(2).toBool();
        // writeRed->insert(ix,alle);
        quantitatot +=qmod->index(row,3).data(0).toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this,QApplication::applicationName(),"ERRORE conversione a double errata",QMessageBox::Ok);
            return;
        }
        //  model->appendRow(createRecipeRow(q.value(1).toString(),q.value(2).toString(),QString::number(q.value(3).toDouble(),'f',2),"","pippo"));
        QList<QStandardItem*> columns;


        QStandardItem* ID_prodotto=new QStandardItem(qmod->index(row,0).data(0).toString());
        QStandardItem* prodotto=new QStandardItem(qmod->index(row,1).data(0).toString());
        QStandardItem* quantita=new QStandardItem(QString::number(qmod->index(row,3).data(0).toDouble(),'f',3));
        QStandardItem* IDLotto=new QStandardItem("");
        QStandardItem* lotto=new QStandardItem("");
        QStandardItem* quadd=new QStandardItem(QString::number(0.0,'f',3));
        QStandardItem* allergene=new QStandardItem(QString::number(qmod->index(row,2).data(0).toBool()));

        if (qmod->index(row,2).data(0).toBool())
        {
            prodotto->setForeground(Qt::red);
            prodotto->setIcon(QIcon(":/Resources/Flag-red64.png"));
        }

        prodotto->setEditable(false);
        lotto->setEditable(false);

        quantita->setEditable(true);

        quadd->setEditable(true);



        //columns.append(IDLotto);
        columns.append(ID_prodotto);
        columns.append(prodotto);
        columns.append(quantita);
        columns.append(IDLotto);
        columns.append(lotto);
        columns.append(quadd);
        columns.append(allergene);


        model->appendRow(columns);


    }

    if (model->rowCount()>0)
    {
        ui->pushButton_10->setEnabled(true);
    }
    else
    {
        ui->pushButton_10->setEnabled(false);
    }

    QString qta=QString::number(quantitatot,'f',3);


    ui->leQtyTotal->setText(qta);
    ui->leQuaRic->setText(qta);



    // qmrighe->setQuery(q);
    ui->tableView->setModel(model);
    //ui->tableView->horizontalHeader()->setStretchLastSection(true);
    // ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->lvRicette->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(calculateActualTotal()));
    connect(ui->tableView->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(productSelected()));
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(calculateActualTotal()));
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnHidden(3,true);
    ui->tableView->setColumnHidden(6,true);
    ui->pushButton_10->setEnabled(true);



}

void HProduction::getNewRow(QList<QStandardItem*>list)
{
    QStandardItemModel* mod=static_cast<QStandardItemModel*>(ui->tableView->model());
    mod->appendRow(list);
}



void HProduction::printProduction()
{
    HPrint *f=new HPrint();
    f->toggleImageUI(false);

    QString title;
    double quanteff=0.0;
    //  printModel=static_cast<QStandardItemModel*>(ui->tableView->model());

    title=ui->lbRicetta->text();
    title.append(" - ");
    title.append(ui->cbClienti->currentText());
    title.append(" - Lotto: " + ui->leNuovoLot->text());
    title.append(QDate::currentDate().toString(" - dd\\MM\\yyyy\n"));


    int rows=model->rowCount();
    int cols=3;

    //QTextTable *table= f->addTable(rows,cols);


    f->cursorToStart();
    f->append(title+"\n",true);
    f->cursorToEnd();


    f->append("NOTE: ",true);
    f->append(ui->tNote->toPlainText());
    f->cursorToEnd();
    f->append("\n");


    f->append(ui->textBrowser->toPlainText(),false);


    f->append("",false);
    f->cursorToEnd();

    QTextTableFormat fmt;
    fmt.setBorder(0.0);
    fmt.setPadding(0.0);
    fmt.setCellPadding(1.0);


    QTextTable *table= f->addTable(rows,cols,fmt);



    QString col1,col2,col3;


    for (int i=0;i<rows;i++)
    {
        quanteff+=model->index(i,5).data(0).toDouble();
        if (ui->checkBox_2->isChecked())
        {
            col1=model->index(i,1).data(0).toString();
            col2=model->index(i,2).data(0).toString();
            //   col3=QString::number(model->index(i,3).data(0).toDouble(),'f',3);
            col3="                                                           ";

        }
        else
        {

            col1=model->index(i,1).data(0).toString();
            col2=model->index(i,4).data(0).toString();
            double r=model->index(i,5).data(0).toDouble();
            col3=QString::number(r,'f',3);


        }

        f->cursorToEnd();

        QTextCharFormat format;


        if(model->index(i,6).data(0).toInt()==1)
        {
            f->writeTableContentRed(table,i,0,format,col1);
            f->writeTableContentRed(table,i,1,format,col2);
            f->writeTableContentRed(table,i,2,format,col3);
        }
        else
        {

            f->writeTableContent(table,i,0,QTextCharFormat(),col1);
            f->writeTableContent(table,i,1,QTextCharFormat(),col2);
            f->writeTableContent(table,i,2,QTextCharFormat(),col3);
        }

        f->cursorToEnd();


    }
    if (!ui->checkBox_2->isChecked())
    {
        f->append("\n\nQuantità: " + ui->leQtyTotal->text(),false);
    }
    else
    {
        f->append("\n\nQuantità: " + ui->leQuaRic->text(),false);
    }
    f->append("",false);


    f->append("\n..............................................................................",false);
    f->cursorToEnd();
    f->append("\n..............................................................................",false);
    f->cursorToEnd();
    f->append("\n..............................................................................",false);

    f->showMaximized();
}

void HProduction::printRecipe()
{
    HPrint *f=new HPrint();
    QString title;


    title=ui->lbRicetta->text();
    title.append(" - ");
    title.append(ui->cbClienti->currentText());


    int rows=ui->tableView->model()->rowCount();
    int cols=2;



    QTextCursor t=f->getCursor();
    t.movePosition(QTextCursor::Start);
    f->cursorToStart();
    f->append(title,false);
    f->append("",false);
    f->cursorToEnd();
    f->append("",false);
    QString col1,col2;
    f->cursorToEnd();

    f->append(ui->textBrowser->toPlainText(),false);

    QTextTable *table= f->addTable(rows,cols,QTextTableFormat());

    for (int i=0;i<rows;i++)
    {

        col1=ui->tableView->model()->index(i,1).data(0).toString();
        col2=ui->tableView->model()->index(i,2).data(0).toString();

        //      f->writeTableContent(table,i,0,col1);

        //     f->writeTableContent(table,i,1,col2);




        // f->append(ui->tvRecipe->model()->index(i,1).data(Qt::DisplayRole).toString() + " - " + ui->tvRecipe->model()->index(i,2).data(Qt::DisplayRole).toString(),false);
        QTextCharFormat format;
        if(model->index(i,6).data(0).toBool())
        {
            f->writeTableContentRed(table,i,0,format,ui->tableView->model()->index(i,3).data(0).toString());
            f->writeTableContentRed(table,i,1,format,QString::number(ui->tableView->model()->index(i,4).data(0).toDouble(),'f',2));
        }
        else
        {
            f->writeTableContent(table,i,0,QTextCharFormat(),ui->tableView->model()->index(i,3).data(0).toString());
            f->writeTableContent(table,i,1,QTextCharFormat(),QString::number(ui->tableView->model()->index(i,4).data(0).toDouble(),'f',2));
        }

        f->cursorToEnd();




    }

    // f->cursorToEnd();

    f->append("QUANTITA': " +ui->leQuaRic->text(),false);

    /*   f->cursorToEnd();







    f->append(ui->textBrowser->toPlainText(),false);*/

    f->toggleImageUI(false);
    f->show();


}

void HProduction::productSelected()
{

    // lastFiveLots();
    //  connect (ui->lvLastLots->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(addLotProd()));


}



void HProduction::on_pushButton_clicked()
{
    // addLotProd();

    //   HLastLots *f=new HLastLots(0,db);
    //   f->show();
}

/* QList<QStandardItem*> HProduction::addTableRow()
 {
     QList<QStandardItem*> list;

 }*/

void HProduction::addLotProd()
{


    //  int idlot;
    //  int prod;
    //  QString descprod;
    //   QString peso;
    QString lotToadd;
    //   QString qty;
    //  QString qp="SELECT descrizione from prodotti where ID="+prod;
    // QList<QString> list;

    //  QStandardItemModel *qm=static_cast<QStandardItemModel*>(ui->tableView->model());

    //lotToadd=ui->leLotToadd->text();

    //   QModelIndex idProdotto =qm->index(ui->tableView->currentIndex().row(),0);
    //  QModelIndex prodotto =qm->index(ui->tableView->currentIndex().row(),1);
    //   QModelIndex quantita =qm->index(ui->tableView->currentIndex().row(),2);
    //  QModelIndex idlotto =model->index(ui->tableView->currentIndex().row(),3);
    //   QModelIndex lotto =model->index(ui->tableView->currentIndex().row(),4);
    //  QModelIndex quanteff =model->index(ui->tableView->currentIndex().row(),5);
    QModelIndex allergene=model->index(ui->tableView->currentIndex().row(),6);

    int row=ui->tableView->currentIndex().row();
    if(row==-1)
    {
        QMessageBox::warning(this,QApplication::applicationName(),"Selezionare il prodotto da aggiungere",QMessageBox::Ok);
        return;
    }

    if(lotToadd=="")
    {
        QMessageBox::warning(this,QApplication::applicationName(),"Selezionare il lotto da aggiungere",QMessageBox::Ok);
        return;
    }

    /*  model->setData(idlotto,ui->lvLastLots->model()->index(ui->lvLastLots->currentIndex().row(),0).data(0).toString());
    model->setData(lotto,ui->lvLastLots->model()->index(ui->lvLastLots->currentIndex().row(),1).data(0).toString());*/
    //  model->setData(quanteff,model->index(row,2).data(0).toString());
    model->setData(allergene,model->index(row,6).data(0).toString());



    //   ui->leLotToadd->setText("");
    //   ui->leqtytoAdd->setText("");
    //   qmLots->appendRow(createRecipeRow(prod));




}

void HProduction::addLotFuoriRicetta()
{
    int prod;
    QString descprod;
    QString peso;
    bool alle=false;
    QString lotToadd;
    double qty=0.0;
    // QStandardItemModel *mod=static_cast<QStandardItemModel*>(ui->tableView->model());


    QList<QString> list;
    QList<QStandardItem*> row;

    // disconnect(ui->lvLastLots->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(lotSelected()));
    //disconnect(ui->lvLastLots->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(addLotProd()));
    //  lotToadd=ui->leLotToadd->text();



    list=lotToadd.split('-');

    prod=list.at(1).toInt();




    QSqlQuery p(db);
    QString qps="SELECT descrizione,allergenico from prodotti where ID=:id";
    p.prepare(qps);
    p.bindValue(":id",QVariant(prod));

    if(p.exec())
    {
        p.first();
        descprod=p.value(0).toString();
        alle=p.value(1).toBool();
    }

    //  peso=ui->leqtytoAdd->text();
    //  qty=ui->leqtytoAdd->text().toDouble();

    qps="SELECT ID from lotdef WHERE lot=:lot";
    p.prepare(qps);
    p.bindValue(":lot",QVariant(lotToadd));
    p.exec();
    p.first();
    int id_lotto=p.value(0).toInt();

    if (id_lotto<1)
    {
        QMessageBox::warning(this,QApplication::applicationName(),"ERRORE\nIl lotto inserito non esiste",QMessageBox::Ok);
        return;
    }
    // qty=QString::number(ui->leqtytoAdd->text(),'g',2);
    //  qty=fabs(ui->leqtytoAdd->text().toDouble());

    QStandardItem *idprodotto=new QStandardItem(QString::number(prod));
    QStandardItem *prodotto=new QStandardItem(descprod);
    if(alle)
    {
        prodotto->setForeground(Qt::red);
        prodotto->setIcon(QIcon(":/Resources/Flag-red64.png"));

    }
    QStandardItem *quantita=new QStandardItem("0.0");
    QStandardItem *idlotto=new QStandardItem(QString::number(id_lotto));
    QStandardItem *lotto =new QStandardItem(lotToadd);
    //QStandardItem *qua=new QStandardItem(ui->tableView->model()->index(ui->tableView->currentIndex().row(),5).data(0).toString());

    // double quag=ui->leqtytoAdd->text().toDouble();
    QStandardItem *qua=new QStandardItem(QString::number(qty,'f',3));
    QStandardItem *allergene=new QStandardItem(QString::number(ui->tableView->model()->index(ui->tableView->currentIndex().row(),6).data(0).toBool()));

    row.append(idprodotto);
    row.append(prodotto);
    row.append(quantita);
    row.append(idlotto);
    row.append(lotto);
    row.append(qua);
    row.append(allergene);
    model->insertRow(ui->tableView->currentIndex().row(),row);
    //  model->appendRow(row);
    //  if(alle) writeRed->insert(ui->tableView->currentIndex().row(),alle);
    // connect(ui->lvLastLots->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(addLotProd()));
    ui->tableView->setCurrentIndex(ui->tableView->model()->index(ui->tableView->currentIndex().row(),0));

    //  ui->leLotToadd->setText("");
    //   ui->leqtytoAdd->setText("");
    recalculateTotal();

}

QString HProduction::getNewLot(int prod)
{
    QSqlQuery qLotQr(db);
    QString sLot="SELECT createID(:sprod)";
    QString ret;
    qLotQr.prepare(sLot);
    qLotQr.bindValue(":sprod",QVariant(prod));
    qLotQr.exec();
    qLotQr.first();
    ret = qLotQr.value(0).toString();

    qDebug()<<ret;
    return ret;
}

void HProduction::addLotFuoriRicettaN(QList<QStandardItem*> row)
{
    /*  QStandardItemModel* mod =static_cast<QStandardItemModel*>(ui->tableView->model());
    mod->appendRow(row);*/
    model->appendRow(row);
}


void HProduction::on_pushButton_5_clicked()
{
    ui->pushButton_3->setEnabled(user->get_produzione_u()>0);
    ui->lvRicette->setEnabled(false);
    ui->leOperatore->setEnabled(false);

    QString nlot=getNewLot(ui->lvRicette->model()->index(ui->lvRicette->currentIndex().row(),1).data(0).toInt());
    ui->leNuovoLot->setText(nlot);



    ui->pushButton_5->setVisible(false);
    ui->pushButton_6->setVisible(true);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    //ui->label->setVisible(true);
    // ui->cbQuanti->setVisible(true);
    // ui->lvLastLots->setVisible(true);
    // ui->leQtyTotal->setReadOnly(true);
    ui->leQtyTotal->setEnabled(true);
    ui->cbTipoLotto->setEnabled(false);
    ui->checkBox->setEnabled(false);
    ui->cbClienti->setEnabled(false);
    ui->leQtyTotal->setFocus();

    getRecipe();
    updateTotals();
   // addPreferredLots();

    //QModelIndex tindex=ui->tableView->selectionModel()->currentIndex();
    ui->tableView->setEnabled(true);
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(addLot(QModelIndex)));



    if(ui->leQtyTotal->text().toDouble()==0.0)
    {
        QMessageBox::warning(this,QApplication::applicationName(),"Inserire la quantità totale da produrre",QMessageBox::Ok);

    }




}

void HProduction::on_pushButton_6_clicked()
{
    /* ui->pushButton_3->setEnabled(false);
    ui->lvRicette->setEnabled(true);
    ui->tableView->setEnabled(false);
    ui->pushButton_5->setVisible(true);
    ui->leOperatore->setEnabled(true);

    ui->pushButton_6->setVisible(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_7->setEnabled(false);*/

    //ui->label->setVisible(false);
    // ui->cbQuanti->setVisible(false);
    // ui->lvLastLots->setVisible(false);
    /*   ui->leNuovoLot->setText("");
    ui->leQtyTotal->setReadOnly(false);
    ui->cbTipoLotto->setEnabled(true);
    ui->checkBox->setEnabled(true);
    ui->cbClienti->setEnabled(true);*/
    // ui->leQtyTotal->setText("0.0");
    //ui->leQtyTotal->setReadOnly(true);
    //   getRecipe();
    //   updateTotals();

    resetForm(false);

    ui->pushButton_3->setEnabled(false);




    //  modifyLot=false;
    //  ui->tableView->setModel(0);
    // getRecipe();
    /* ui->pushButton_10->setEnabled(false);
    ui->lvRicette->setCurrentIndex(ui->lvRicette->model()->index(0,0));
    ui->lvRicette->selectionModel()->select(ui->lvRicette->model()->index(0,0),QItemSelectionModel::Select);*/
    //  ui->tableView->setEnabled(false);
    //  disconnect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(addLot(QModelIndex)));

}

bool HProduction::saveNewLot(QString lot, int prodotto)
{
    bool b;
    QSqlQuery q(db);

    // qDebug()<<lot<<prodotto<<data<<"saveNewLot()";

    QString sql="INSERT INTO `lotdef`(`lot`,`prodotto`,`data`,`giacenza`,`um`,`scadenza`,`anagrafica`,`lot_fornitore`, `EAN`, `tipo`, `attivo`,`note`,`operatore`) VALUES(:lot,:prodotto,:data,:giacenza,:um,:scadenza ,:anagrafica,:lotf,:ean,:tipo,:attivo,:note,:operatore)";
    QString giacenza=ui->leQtyTotal->text();
    QDate scadenza=ui->dateEdit->date();
    // qDebug()<<scadenza.toString("yyyy-MM-dd");
    QString anagrafica;
    if(ui->checkBox->isChecked())
    {
        anagrafica=ui->lvSubclienti->model()->index(ui->lvSubclienti->currentIndex().row(),0).data(0).toString();
    }
    else
    {
        anagrafica=ui->cbClienti->model()->index(ui->cbClienti->currentIndex(),0).data(0).toString();
    }
    QString lotf="PRODUZIONE";
    QString ean="";
    QString tipo="3"/*ui->cbTipoLotto->model()->index(ui->cbTipoLotto->currentIndex(),0).data(0).toString();*/;
    QString attivo="2";
    QString um=ui->cbUm->model()->index(ui->cbUm->currentIndex(),0).data(0).toString();
    QString note=ui->tNote->toPlainText();
    QDateTime data=QDateTime::currentDateTime();
    QString oper=ui->leOperatore->text();

    b=false;

    if(QMessageBox::question(this,QApplication::applicationName(),"Confermare la produzione?",QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        q.prepare(sql);

        q.bindValue(":lot",QVariant(lot));
        q.bindValue(":prodotto",QVariant(prodotto));
        q.bindValue(":data",QVariant(data));
        q.bindValue(":giacenza",QVariant(giacenza));
        q.bindValue(":um",QVariant(um));
        q.bindValue(":scadenza",QVariant(scadenza));
        q.bindValue(":anagrafica",QVariant(anagrafica));
        q.bindValue(":lotf",QVariant(lotf));
        q.bindValue(":ean",QVariant(ean));
        q.bindValue(":tipo",QVariant(tipo));
        q.bindValue(":attivo",QVariant(attivo));
        q.bindValue(":note",QVariant(note));
        q.bindValue(":operatore",QVariant(oper));
        b = q.exec();

        if(!b){

            QMessageBox::warning(this,QApplication::applicationName(),"errore ell'inserimento nuovo lotto"+db.lastError().text(),QMessageBox::Ok);
        }

    }else{

        QMessageBox::warning(this,QApplication::applicationName(),"Produzione annullata"+db.lastError().text(),QMessageBox::Ok);

    }

    return b;


}
//--saveNewLot()-------------------------------

bool HProduction::saveLotLoad(int idlotto, int prodotto)
{


    QSqlQuery q(db);

    QString sql="INSERT INTO `operazioni`(`IDlotto`,`data`,`utente`,`IDprodotto`,`azione`,`quantita`,`um`)VALUES(:idl,:data,:utente,:prodotto,:azione,:quantita,:um)";


    double giacenza=ui->leQtyTotal->text().toDouble();
    int um=ui->cbUm->model()->index(ui->cbUm->currentIndex(),0).data(0).toInt();
    QDateTime data=QDateTime::currentDateTime();

    q.prepare(sql);
    q.bindValue(":idl",QVariant(idlotto));
    q.bindValue(":data",QVariant(data));
    q.bindValue(":utente",QVariant(user->getID()));
    q.bindValue(":prodotto",QVariant(prodotto));
    q.bindValue(":azione",QVariant(1));
    q.bindValue(":quantita",QVariant(giacenza));
    q.bindValue(":um",QVariant(um));
    qDebug()<<"prima di eseguire la query di lotload";
    bool b=q.exec();
    qDebug()<<q.lastError().text()<<q.lastQuery();
    if(!b){
        db.rollback();
    }

    return b;
}
//--saveLotLoad()------------------------------

int HProduction::lastInsertId()
{
    QSqlQuery q(db);
    QString sql="SELECT last_insert_id();";
    q.prepare(sql);
    q.exec();

    q.first();

    return q.value(0).toInt();


}

bool HProduction::saveOperation(int row,int action)//carico=1 - scarico= 2
{
    QSqlQuery q(db);
    QString sql;

    QString data,note;


    int idlotto=ui->tableView->model()->index(row,3).data(0).toInt();
    int idprodotto=ui->tableView->model()->index(row,0).data(0).toInt();
    int utente=user->getID();
    double quant= ui->tableView->model()->index(row,5).data(0).toDouble();
    int um=ui->cbUm->model()->index(ui->cbUm->currentIndex(),0).data(0).toInt();

    data=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    note="";


    sql="INSERT INTO `operazioni` (`IDlotto`,`data`,`utente`, `IDprodotto`,`azione`,`quantita`,`um`) VALUES (:idlotto,:data,:utente,:idprodotto,:azione,:quantita,:um)";
    q.prepare(sql);

    q.bindValue(":idlotto",QVariant(idlotto));
    q.bindValue(":data",QVariant(data));
    q.bindValue(":utente",QVariant(utente));
    q.bindValue(":idprodotto",QVariant(idprodotto));
    q.bindValue(":azione",QVariant(action));
    q.bindValue(":quantita",QVariant(quant));
    q.bindValue(":um",QVariant(um));


    bool b=false;

    b=q.exec();

    if(!b)
    {
        qDebug() << q.lastError().text();
        // QMessageBox::warning(this,QApplication::applicationName(),"Errore in saveOperazione(" + QString::number(row) +")\nIDlotto:"+idlotto,QMessageBox::Ok);

    }

    return b;
}


//---saveOperation

//*----save Composizione----------------

bool HProduction::saveComposizione(int lottotarget,int operazione)
{
    bool b=false;

    QSqlQuery q(db);
    QString sql;

    // int operazione=lastInsertId();

    // #include <QDebug><<"lottotarget: "<<QString::number(lottotarget)<<" operazione: "<<QString::number(operazione);
    sql="INSERT INTO composizione_lot (ID_lotto,operazione) VALUES (:lottotarget,:operazione)";
    q.prepare(sql);
    q.bindValue(":lottotarget",QVariant(lottotarget));
    q.bindValue(":operazione",QVariant(operazione));
    // qDebug()<<q.boundValue(0).toString()<<q.boundValue(1).toString();

    b=q.exec();
    // qDebug()<< "savecomposizione"<<q.lastError().text();

    return b;

}
//-------- saveComposition();

bool HProduction::saveProduction()
{
    QSqlQuery q(db);
    QSqlQuery qop(db);
    QString sql;
    QString lotto,utente;
    int idprodotto;
    int newlotid;
    QDateTime data;
    QDate scadenza;
    int op;

    bool fb=false;

    data=QDateTime::currentDateTime();
    scadenza=ui->dateEdit->date();
    idprodotto=ui->lvRicette->model()->index(ui->lvRicette->currentIndex().row(),1).data(0).toInt();






    //creo un nuovo lotto

    db.transaction();

    lotto=getNewLot(idprodotto);

    qDebug()<<lotto;

    if(QMessageBox::warning(this,QApplication::applicationName(),"Avviare il salvataggio della produzione?\n verificare",QMessageBox::Ok| QMessageBox::Cancel)==QMessageBox::Cancel)
    {
        db.rollback();
        ui_enable(ACTION::WAIT);
        return false;
    }

   //  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    fb=saveNewLot(lotto,idprodotto);
    qDebug()<<"saveNewLot"<<fb;

    if(!fb)
    {
        db.rollback();
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this,QApplication::applicationName(),"Il nuovo lotto non è stato creato\nOperazione annullata o fallita"+db.lastError().text(),QMessageBox::Ok);

        return fb;
    }


    newlotid=lastInsertId();

    //carico il nuovo lotto
    fb=saveLotLoad(newlotid,idprodotto);
    if(!fb)
    {
        QMessageBox::warning(this,QApplication::applicationName(),"Errore in SaveLotLoad"+db.lastError().text(),QMessageBox::Ok);
        db.rollback();
        QApplication::restoreOverrideCursor();
        return fb;
    }

    //scarico i componenti e salvo la composizione del lotto
    int rows=ui->tableView->model()->rowCount();

    bool bop=false;
    for (int x=0;x<rows;x++)
    {

        bop=saveOperation(x,2);
        // // qDebug()<<"saveoperazione"<<QString::number(x);
        op=lastInsertId();
        // recupero l'id dell'ultima operazione di scarico(cioè lo scarico della prima riga di  tableview se x=0
        if (bop)
        {
            //se l'operazione è salvata senza errori
            // la salvo in composizone_lot,new lotid è il lotto di cui voglio salvare la composizione (lotid) e op è l'id dell'operazione
            bool j=saveComposizione(newlotid,op);

            op=lastInsertId();
            if (!j)
            {
                QMessageBox::warning(this,QApplication::applicationName(),"Errore in saveComposizione",QMessageBox::Ok);
                db.rollback();
                QApplication::restoreOverrideCursor();
                return j;
            }

        }
        else
        {

            QMessageBox::warning(this,QApplication::applicationName(),"Errore in saveOperation("+ QString::number(x) +")",QMessageBox::Ok);
            db.rollback();
            QApplication::restoreOverrideCursor();
            return bop;
        } //se saveoperazione



    }//for (per ogni riga della tabella)


    if(bop)
    {
         QApplication::restoreOverrideCursor();
        db.commit();
        QMessageBox::information(this,QApplication::applicationName(),"Produzione salvata",QMessageBox::Ok);
        ui->lvRicette->setEnabled(true);
        ui->pushButton_5->setVisible(true);
        ui->pushButton_6->setVisible(false);
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_7->setEnabled(false);
        ui->pushButton_3->setEnabled(false);

    }
    else
    {

        QApplication::restoreOverrideCursor();
        db.rollback();
        QMessageBox::information(this,QApplication::applicationName(),"Errore",QMessageBox::Ok);
    }

    ui->tableView->setEnabled(false);

    QApplication::restoreOverrideCursor();



    return bop;


}

bool HProduction::saveUpdatedComposizione()
{

    // int rows=ui->tableView->model()->rowCount();
    bool b=false;

    QSqlQuery q(db);
    QString sql;

    int operazione=lastInsertId();


    sql="INSERT INTO composizione_lot (ID_lotto,operazione) VALUES (:lottotarget,:operazione)";
    q.prepare(sql);
    q.bindValue(":lottotarget",QVariant(lottomodifica));
    q.bindValue(":operazione",QVariant(operazione));

    b=q.exec();
    // qDebug()<< "savecomposizione"<<QString::number(b);

    return b;
}




void HProduction::on_pushButton_3_clicked()
{


    if (!modifyLot){

        saveProduction();

    }
    else
    {
        updateComposition();
    }

    resetForm(true);

    ui->pushButton_3->setEnabled(false);


}



void HProduction::on_leQtyTotal_returnPressed()
{

    updateTotals();

}

void HProduction::on_pushButton_7_clicked()
{

    QString product=ui->tableView->model()->index(ui->tableView->selectionModel()->currentIndex().row(),1).data(0).toString();
    HLastLots *f=new HLastLots(nullptr,db,ui->tableView->model()->index(ui->tableView->selectionModel()->currentIndex().row(),2).data(0).toDouble(),product);
    // connect(f,SIGNAL(rowAdded(QList<QStandardItem*> row)),this,SLOT(addLotFuoriRicettaN(QList<QStandardItem*> row)));
    connect (f,SIGNAL(rowAdded(QList<QStandardItem*>)),this,SLOT(addLotFuoriRicettaN(QList<QStandardItem*>)));
    f->show();
}

void HProduction::on_pushButton_4_clicked()
{
    if(QMessageBox::question(this,QApplication::applicationName(),"Chiudere la finestra?",QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
    {
        this->close();
    }
}

void HProduction::on_pushButton_2_clicked()
{
    if(QMessageBox::question(this,QApplication::applicationName(),"Eliminare la riga?",QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
    {
        QModelIndex ix=ui->tableView->currentIndex();
        //  QModelIndex xx=ui->tableView->model()->index(ix.row(),2);
        // QModelIndex xy=ui->tableView->model()->index(ix.row(),5);

        ui->tableView->model()->removeRow(ix.row());
        //  ui->tableView->model()->setData(xx,QVariant(0),Qt::EditRole);
        //  ui->tableView->model()->setData(xy,QVariant(0),Qt::EditRole);
        recalculateTotal();

    }
}

void HProduction::on_pushButton_7_toggled(bool checked)
{
    setAddProductFuoriRicettaUI(checked);
}

void HProduction::setAddProductFuoriRicettaUI(bool visible)
{
    ui->label_6->setVisible(visible);
    /* ui->label_2->setVisible(visible);
    ui->label_4->setVisible(visible);
    ui->leqtytoAdd->setVisible(visible);
    ui->leLotToadd->setVisible(visible);*/
    //  ui->cbTipoLotto->setVisible(visible);
    ui->pbAddLottoFuoriRicetta->setVisible(visible);
    ui->pbAnnulla->setVisible(visible);

}

void HProduction::on_pbAddLottoFuoriRicetta_clicked()
{


    addLotFuoriRicetta();
    setAddProductFuoriRicettaUI(false);
    ui->pushButton_7->setChecked(false);
}


bool HProduction::updateComposition()
{

    // int rows=ui->tableView->model()->rowCount();

    //  int idprodotto;
    //  double quantita;
    //  int idlotto;
    // double quanteff;
    //   int idoperazione;

    QString sql;
    QSqlQuery q(db);

    //cancello la composizione del lotto
    sql="delete from composizione_lot where ID_lotto=:lotto";

    q.prepare(sql);
    q.bindValue(":lotto",QVariant(lottomodifica));
    q.exec();
    if (!q.exec())
    {
        QMessageBox::warning(this,QApplication::applicationName(),"errore aggiornando la composizione (step 1)",QMessageBox::Ok);
        db.rollback();
        return false;
    }


    bool fb=false;

    //salvo le operazioni nuove

    for (int x=0;x<ui->tableView->model()->rowCount();x++)
    {
        fb=saveUpdatedOperazione(x);
        if (fb)
        {
            bool j=saveUpdatedComposizione();
            if (!j)
            {
                QMessageBox::warning(this,QApplication::applicationName(),"Errore in saveUpdatedComposizione",QMessageBox::Ok);
                return j;
            }
        }
        else
        {
            QMessageBox::warning(this,QApplication::applicationName(),"Errore in saveUpdatedOperation("+ QString::number(x) +")",QMessageBox::Ok);


        }



    }

    return fb;





}

bool HProduction::saveUpdatedOperazione(int row)
{
    bool b;
    QSqlQuery q(db);
    QString sql;

    QString data,azione,note;


    int idlotto=ui->tableView->model()->index(row,3).data(0).toInt();
    int idprodotto=ui->tableView->model()->index(row,0).data(0).toInt();
    int utente=user->getID();
    double quant= ui->tableView->model()->index(row,5).data(0).toDouble();
    int um=ui->cbUm->model()->index(ui->cbUm->currentIndex(),0).data(0).toInt();

    data=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    azione="2";
    note="";


    sql="INSERT INTO `operazioni` (`IDlotto`,`data`,`utente`, `IDprodotto`,`azione`,`quantita`,`um`,`note`) VALUES (:idlotto,:data,:utente,:idprodotto,:azione,:quantita,:um,:note)";
    q.prepare(sql);

    q.bindValue(":idlotto",QVariant(idlotto));
    q.bindValue(":data",QVariant(data));
    q.bindValue(":utente",QVariant(utente));
    q.bindValue(":idprodotto",QVariant(idprodotto));
    q.bindValue(":azione",QVariant(azione));
    q.bindValue(":quantita",QVariant(quant));
    q.bindValue(":um",QVariant(um));
    q.bindValue(":note",QVariant(note));


    b=q.exec();



    return b;


}

void HProduction::resetForm(bool pcomplete)
{
    ui->dateEdit->setVisible(false);
    ui->label_9->setVisible(false);
   // ui->pushButton_10->setEnabled(false);
    ui->pushButton->setVisible(false);
    ui->checkBox->setVisible(false);
   // ui->cbClienti->setCurrentIndex(ui->cbClienti->model()->rowCount());
   // ui->cbClienti->setCurrentIndex(0);

    ui->lvRicette->setEnabled(true);
    ui->lvSubclienti->setVisible(false);

    ui->pushButton_5->setEnabled(user->get_produzione_u()>0);
    ui->pushButton_2->setEnabled(user->get_produzione_u()>0);
    ui->pushButton_3->setEnabled(user->get_produzione_u()>0);
    ui->pushButton_7->setEnabled(user->get_produzione_u()>0);
    ui->pushButton_11->setEnabled(user->get_produzione_u()>0);

    ui->leQtyTotal->setEnabled(true);
    ui->pushButton_5->setVisible(true);
    ui->pushButton_6->setVisible(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
     ui->leNuovoLot->setText("");
    ui->label_6->setVisible(true);
    ui->dateEdit->setDate(QDate::currentDate().addYears(2));
    if(pcomplete) ui->leOperatore->setText(QString());
    ui->leOperatore->setEnabled(true);

    ui->cbClienti->setEnabled(true);

    setAddProductFuoriRicettaUI(false);

    modifyLot=false;

}

void HProduction::on_pushButton_10_clicked()
{
    printProduction();
}

void HProduction::on_checkBox_toggled(bool checked)
{
    if (checked)
    {
        getSubclients();
        ui->lvSubclienti->setVisible(true);
        getRecipesForClient();
    }
    else
    {
        ui->lvSubclienti->setVisible(false);
        getRecipesForClient();
    }
}

void HProduction::on_pbAnnulla_clicked()
{

}

void HProduction::on_pushButton_11_clicked()
{
    if(QMessageBox::question(0,QApplication::applicationDisplayName(),"Resettare la ricetta?",QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
    {
        getRecipe();
    }
}



void HProduction::ui_enable(int arg)
{


    if (arg==ACTION::RESET)
    {

        ui->lvRicette->setEnabled(true);
        ui->pushButton_5->setVisible(true);
        ui->pushButton_6->setVisible(false);
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_7->setEnabled(false);
        ui->leNuovoLot->setText("");
        ui->leQtyTotal->setReadOnly(false);
        ui->cbTipoLotto->setEnabled(true);
        ui->checkBox->setEnabled(true);
        ui->cbClienti->setEnabled(true);
    }


}

void HProduction::addPreferredLots()
{
   int rows=ui->tableView->model()->rowCount();

   for(int row=0;row<rows;++row)
   {
       QModelIndex ix=ui->tableView->model()->index(row,0);
       addLot(ix,false);
   }
}

const QString HProduction::findPreferredLot(const int id_prod)
{
    QString default_lot=QString();
    QString sql("SELECT lot FROM pref WHERE prod=:prod");
    QSqlDatabase prefdb=QSqlDatabase::addDatabase("QSQLITE");
    prefdb.setDatabaseName("preferences.db");
    prefdb.open();

    QSqlQuery q(prefdb);
    q.prepare(sql);
    q.bindValue(":prod",id_prod);
    if (q.exec())
    {
        q.first();
        default_lot=q.value(0).toString();
    }

    prefdb.close();

    return default_lot;
}


void HProduction::addLot(QModelIndex index,bool show_window)
{
    HDataToPass *data=new HDataToPass(0);


    data->allergene=ui->tableView->model()->index(index.row(),6).data(0).toBool();
    data->description=ui->tableView->model()->index(index.row(),1).data(0).toString();
    data->productId=ui->tableView->model()->data(ui->tableView->model()->index(index.row(),0)).toInt();
    data->row=index.row();
    data->quantity=ui->tableView->model()->index(index.row(),5).data(0).toDouble();
    data->mod=static_cast<QStandardItemModel*>(ui->tableView->model());


    HAddLotInProduction *f= new HAddLotInProduction(data,db);

    if(show_window){
   // f->setWindowModality(Qt::ApplicationModal);
    f->move(10+ui->tableView->x()-f->width()-10,QCursor::pos().y());
    f->show();


    // f->setWindowFlags(Qt::FramelessWindowHint);





    }else{
        f->click();
        f->close();
    }

}





void HProduction::on_pbPreferredLots_clicked()
{
    addPreferredLots();
}

