#include "hrecipeaddrow.h"
#include "ui_hrecipeaddrow.h"

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QList>
#include <QStandardItem>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>


HRecipeAddRow::HRecipeAddRow(int pidricetta, QSqlDatabase pdb, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HRecipeAddRow)
{
    ui->setupUi(this);
    db=pdb;

    QSqlTableModel *mod= new QSqlTableModel(0,db);
    mod->setTable("prodotti");
    mod->setFilter("tipo=1");
    mod->setSort(1,Qt::AscendingOrder);
    mod->select();

    ui->checkBox->setVisible(false);

    ui->comboBox->setModel(mod);
    ui->comboBox->setModelColumn(1);

    idricettatarget=pidricetta;
}

HRecipeAddRow::~HRecipeAddRow()
{
    delete ui;
}

void HRecipeAddRow::on_pushButton_2_clicked()
{
    close();
}

void HRecipeAddRow::on_pushButton_clicked()
{

    addRecipeRow();

}
void HRecipeAddRow::addRecipeRow()
{
  //  bool ok;
    QList<QStandardItem*> list;
    double quant=0.0;
    /*
    q.bindValue(":idricetta",ui->cbRicette->model()->index(ui->cbRicette->currentIndex(),0).data(0));
    q.bindValue(":idprodotto",ui->cbProdotti->model()->index(ui->cbProdotti->currentIndex(),0).data(0));
    q.bindValue(":qua",QVariant(qua));
    q.bindValue(":show",QVariant(ui->checkBox->isChecked()));
    */
    QStandardItem* id= new QStandardItem(QString::number(-1));
    QStandardItem* ricetta= new QStandardItem(QString::number(idricettatarget));
    QStandardItem* prodotto=new QStandardItem  (QString::number(ui->comboBox->model()->index(ui->comboBox->currentIndex(),0).data(0).toInt()));
    QStandardItem* descrizione=new QStandardItem(ui->comboBox->currentText());
    bool ok;
    QString sQuan=QString::number(ui->lineEdit->text().toDouble(&ok),'f',4);
    QStandardItem* quantita=new QStandardItem(sQuan);
    qDebug()<<sQuan;
    if(!ok)
    {
        QMessageBox::warning(0,QApplication::applicationName(),"errore nella quantità",QMessageBox::Ok);
        return;
    }
    QString show;
    ui->checkBox->isChecked()? show="1" : show="0";
    QStandardItem* showprod=new QStandardItem(show);
    //QStandardItem* descrizione3=new QStandardItem("pippo3");


    list.append(id);
    list.append(ricetta);
    list.append(prodotto);
    list.append(descrizione);
    list.append(quantita);
    list.append(showprod);


    emit rowadded(list);

    close();



  /*  QSqlQuery q(db);
    QString sql="INSERT INTO righe_ricette(`ID_ricetta`,`ID_prodotto`,`quantita`,`show_prod`)VALUES(:idricetta,:idprodotto,:qua,:show)";
    //double qua=ui->lineEdit->text().toDouble(&quack);


    q.prepare(sql);
    q.bindValue(":idricetta",QVariant(idricettatarget));
    q.bindValue(":idprodotto",QVariant(ui->comboBox->model()->index(ui->comboBox->currentIndex(),0).data(0).toInt()));
    q.bindValue(":qua",QVariant(sQuan));
    q.bindValue(":show",QVariant(ui->checkBox->isChecked()));
    if(q.exec())
    {
    emit rowadded(list);
     }
    else
    {

            QMessageBox::warning(this,QApplication::applicationName(),q.lastError().text(),QMessageBox::Ok);
            return;

    }*/





}

void HRecipeAddRow::on_lineEdit_returnPressed()
{
    bool ok;
    double quant=ui->lineEdit->text().toDouble(&ok);
    if(!ok)
    {
        QMessageBox::warning(this,QApplication::applicationName(),"Errore! Attenzione, verificare la quantità",QMessageBox::Ok);
    }
}
