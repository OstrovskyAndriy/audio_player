#ifndef USERS_H
#define USERS_H

#include <QDialog>
#include "mainwindow.h"
#include <QSqlTableModel>
#include <adduser.h>
#include<QMessageBox>

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


    void on_addUser_clicked();

    void on_passwordEdit_returnPressed();

    void showPassword();
    void hidePassword();

private:
    Ui::Users *ui;

    MainWindow *playerWindow;
    DBManager *dbManager;
    QSqlTableModel *model;
    AddUser *addUser;
    QMessageBox errorMsg;
};

#endif // USERS_H
