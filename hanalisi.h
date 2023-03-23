#ifndef HANALISI_H
#define HANALISI_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include "huser.h"
#include <QDate>

namespace Ui {
class HAnalisi;
}

class HAnalisi : public QWidget
{
    Q_OBJECT

public:
    explicit HAnalisi(QSqlDatabase pdb=QSqlDatabase(), HUser *p_user=new HUser(), QWidget *parent = nullptr);
    ~HAnalisi();
public slots:


private:
    Ui::HAnalisi *ui;
    QSqlDatabase db;
    QSqlTableModel *tmClienti;
    HUser *user;
    QDate datedal;
    QDate dateal;


private slots:

    void getProductsForClient();
    void getLotComponents();
    void getLotComponents(int idlotto);
    int getLotID(QString lot);
    //  void findLotComposition();
    void findLotUse();
    void setLotSearch(QString msg);
    void setNarrowSearch();
    void narrow();
    void setCompSearch();
    void on_rbAll_toggled(bool checked);
    // void on_rbMateriePrime_toggled(bool checked);
    void on_rbProdottifiniti_toggled(bool checked);
    // void on_rbSemilavorati_toggled(bool checked);
    void on_rbPackages_toggled(bool checked);
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void getYearlyProduction();
    void on_checkBox_toggled(bool checked);
    void on_deFrom_dateChanged(const QDate &date);
    void on_deTo_dateChanged(const QDate &date);
    void printLot();
    void printThis();
    void on_pbPrint_clicked();
    void doMenu(QPoint pos);
    void on_tvLots_doubleClicked(const QModelIndex &index);
    void on_pbBioData_clicked();
};

#endif // HANALISI_H
