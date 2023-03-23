#ifndef HASSOCIAZIONI_H
#define HASSOCIAZIONI_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QCompleter>
#include <QItemDelegate>
#include "huser.h"


namespace Ui {
class HAssociazioni;
}

class HAssociazioni : public QWidget
{
    Q_OBJECT

public:
    explicit HAssociazioni(HUser *puser,QSqlDatabase pdb,QWidget *parent = 0);
    ~HAssociazioni();
    void setVisibility(bool visible);

private:
    Ui::HAssociazioni *ui;

    QSqlDatabase db;
    QSqlTableModel *tmClienti;
    QCompleter *comp;
    QSqlQueryModel *qm;
    QSqlQueryModel *tvqm;
    QItemDelegate *delegate;
    HUser *user;
private slots:
    void getRecipes();
    void getRecipesForTable();
    void saveAssociation();
    void deleteAssociation();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pbPrint_clicked();
    void print();
};

#endif // HASSOCIAZIONI_H
