#ifndef ADDUSER_H
#define ADDUSER_H

#include <QDialog>
#include <dbmanager.h>
#include <QMessageBox>

namespace Ui {
class AddUser;
}

class AddUser : public QDialog
{
    Q_OBJECT

public:
    explicit AddUser(QWidget *parent = nullptr);
    ~AddUser();

    void addUser();
    void openErrorMsg();

private:
    Ui::AddUser *ui;
    DBManager *dbManager;
    QMessageBox error;
};

#endif // ADDUSER_H
