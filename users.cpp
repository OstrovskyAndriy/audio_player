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
    ui->addUser->setFocusPolicy(Qt::NoFocus);
    vievOfTable();
}

Users::~Users()
{
    delete ui;
}


void Users::vievOfTable()
{
    model->setTable(dbManager->getUsersTableName());
    model->select();
    ui->usersTableView->setModel(model);
    ui->usersTableView->hide();

    ui->usersTableView->hideColumn(0);
    ui->usersTableView->hideColumn(2);
    ui->usersTableView->setColumnWidth(1,ui->usersTableView->width());
}


void Users::on_addUser_clicked()
{
    addUser=new AddUser;
    addUser->show();
    connect(addUser->window(),&AddUser::destroyed,this,&Users::vievOfTable);
}


void Users::on_usersTableView_doubleClicked(const QModelIndex &index)
{
    playerWindow=new MainWindow(index.row()+1);
    qDebug()<<index.row()+1;

    this->hide();
    playerWindow->show();
}


void Users::on_passwordEdit_returnPressed()
{
   qDebug()<<"pressed";
   const QString name = ui->nameEdit->text();
   const QString password = ui->passwordEdit->text();
   QString nameInTable;
   QString pswrdIntable;

   for(int i=0;i<ui->usersTableView->model()->rowCount();i++){
       nameInTable = ui->usersTableView->model()->data(ui->usersTableView->model()->index(i,1)).toString();
       pswrdIntable = ui->usersTableView->model()->data(ui->usersTableView->model()->index(i,2)).toString();

       if(name==nameInTable||password==pswrdIntable){
           qDebug()<<"user finded";

           playerWindow=new MainWindow(i+1);
           qDebug()<<i+1;

           this->hide();
           playerWindow->show();

           return;
       }
   }

   errorMsg.setText("Incorrect name or password \nor user not found");
   errorMsg.show();
}

