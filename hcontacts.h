#ifndef HCONTACTS_H
#define HCONTACTS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QStringListModel>
#include "huser.h"

namespace Ui {
class HContacts;
}

class HContacts : public QWidget
{
    Q_OBJECT

public:
    explicit HContacts(QWidget *parent = 0,HUser* pusr=0,QSqlDatabase pdb=QSqlDatabase());
    ~HContacts();

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void on_pushButton_5_clicked();

    bool removeContact();

    void on_pushButton_2_clicked();

    void reload();

    void on_pushButton_6_clicked();

    void on_lineEdit_3_textChanged(const QString &arg1);

    void on_rbNome_toggled(bool checked);

private:
    Ui::HContacts *ui;
    QSqlDatabase db;
    QSqlTableModel *mod;
    QStringListModel *tipomod;
    HUser *user;


};

#endif // HCONTACTS_H
