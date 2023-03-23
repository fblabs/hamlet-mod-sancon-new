#ifndef HLOTTI_H
#define HLOTTI_H

#include <QWidget>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
//----------------------------------------
#include <QSqlQueryModel>
//----------------------------------------
#include <QSqlTableModel>
#include <QSqlDatabase>
#include "hreadonlymodellots.h"
#include "huser.h"
#include <QShortcut>
#include <QDate>
#include "hnuovaoperazione.h"
#include <QPrinter>

namespace Ui {
class HLotti;
}

class HLotti : public QWidget
{
    Q_OBJECT

public:

    explicit HLotti(QSqlDatabase pdb,HUser *puser,QWidget *parent = 0);
    ~HLotti();

 //   void setConnectionName(QString conn);


private slots:
    void getLotsOverview();
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void searchProduct();

  //  void onConnectionNameSet();

    void resetData();
    void resetFilter(QString prevFilter);

    void on_pushButton_4_clicked();

   // void on_pushButton_5_clicked();

    void setFilter();
    void showContextMenu(const QPoint& pos);
    void copyField();
    void editLot();
    void print(bool pdf);
    void updateTableView();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void getDetails();
    void modifySelected(int pidlotto=-1 );
    void deleteSelectedLot();
    void dataBio();

    void printPreview(QPrinter *printer);
    void printPreviewSlot(QString filename=QString());

   // void on_leLottoRaw_textChanged(const QString &arg1);

    void on_chTipoProdotti_toggled(bool checked);

    void on_cbTipoProd_currentIndexChanged(int index);

    void on_chbT_toggled(bool checked);

    void on_cbTipiLot_currentIndexChanged(int index);

    void on_chbP_toggled(bool checked);

    void on_cbProdotti_currentIndexChanged(int index);

    void on_twLots_doubleClicked(const QModelIndex &index);

 //   void on_datadal_dateChanged(const QDate &date);

   // void on_dataal_dateChanged(const QDate &date);

   void on_pbScadenze_clicked();

   void on_datadal_dateChanged(const QDate &date);

   void on_dataal_dateChanged(const QDate &date);

   void getDataLots();

   void on_pbRefresh_clicked();

   void on_twLots_clicked(const QModelIndex &index);


   void on_chBio_toggled(bool checked);


   void on_pbDeleteLot_clicked();

   void on_leOperatore_returnPressed();


   void on_leLottoRaw_returnPressed();

   void on_leLottoRaw_textEdited(const QString &arg1);

private:

    Ui::HLotti *ui;
    QSqlDatabase db;
    QSqlRelationalTableModel *model;

    HUser *user;
    HReadOnlyModelLots*tbm;
    QSqlTableModel *mTipi;
    QSqlTableModel *mProdotti;
    QSqlTableModel *mTipiProdotto;
    QShortcut *det;
    QString filter;
    QDate dal;
    QDate al;
    bool dateset;
    QString prevFilter;
    QSqlQueryModel *mod;







};

#endif // HLOTTI

