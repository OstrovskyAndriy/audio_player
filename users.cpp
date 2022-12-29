#include "users.h"
#include "ui_users.h"

Users::Users(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Users)
{
    ui->setupUi(this);

    dbManager=DBManager::getInstance();
    dbManager->connectToDataBase();

    model =new QSqlTableModel(this,dbManager->getDB());

    vievOfTable();
}

Users::~Users()
{
    delete ui;
}

void Users::on_openPlayer_clicked()
{
    playerWindow=new MainWindow(1);

    this->hide();
    playerWindow->show();
}

void Users::vievOfTable()
{
    model->setTable(dbManager->getUsersTableName());
    ui->usersTableView->setModel(model);

    ui->usersTableView->hideColumn(0);
    ui->usersTableView->hideColumn(2);
    ui->usersTableView->setColumnWidth(1,ui->usersTableView->width());
}

