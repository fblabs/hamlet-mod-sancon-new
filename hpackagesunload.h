#ifndef HPACKAGESUNLOAD_H
#define HPACKAGESUNLOAD_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include "huser.h"

namespace Ui {
class HPackagesUnload;
}

class HPackagesUnload : public QWidget
{
    Q_OBJECT

public:
    explicit HPackagesUnload(HUser* puser=0, QSqlDatabase pdb=QSqlDatabase(),QWidget *parent = 0);
    ~HPackagesUnload();

private:
    Ui::HPackagesUnload *ui;
    QSqlDatabase db;
    HUser* user;
    QString baseFilter;
    QSqlTableModel *modClienti;
    QSqlQueryModel *modProdotti;
    QSqlTableModel *mlots;
private slots:
    void getClients();
    void getProducts();
    void loadPackages();
    void on_pushButton_2_clicked();
    void on_cbClienti_currentIndexChanged(int index);
    bool scarica();
    void on_pushButton_clicked();
    void setQuantitaText();
signals:
    void update();
};

#endif // HPACKAGESUNLOAD_H
