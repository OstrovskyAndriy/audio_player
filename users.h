#ifndef USERS_H
#define USERS_H

#include <QDialog>
#include "mainwindow.h"
#include <QSqlTableModel>

namespace Ui {
class Users;
}

class Users : public QDialog
{
    Q_OBJECT

public:
    explicit Users(QWidget *parent = nullptr);
    ~Users();

private slots:
    void on_openPlayer_clicked();

    void vievOfTable();

private:
    Ui::Users *ui;

    MainWindow*playerWindow;
    DBManager*dbManager;
    QSqlTableModel *model;

};

#endif // USERS_H
