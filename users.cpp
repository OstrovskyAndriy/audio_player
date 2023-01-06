#include "users.h"
#include "ui_users.h"

Users::Users(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Users)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().width(),this->geometry().height());

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


void Users::on_passwordEdit_returnPressed()
{
   const QString name = ui->nameEdit->text();
   const QString password = ui->passwordEdit->text();
   QString nameInTable;
   QString pswrdIntable;

   for(int i=0;i<ui->usersTableView->model()->rowCount();i++){

       nameInTable = ui->usersTableView->model()->data(ui->usersTableView->model()->index(i,1)).toString();
       pswrdIntable = ui->usersTableView->model()->data(ui->usersTableView->model()->index(i,2)).toString();

       if(name==nameInTable||password==pswrdIntable){
           playerWindow=new MainWindow(i+1);

           this->hide();
           playerWindow->show();
           connect(playerWindow->window(),&MainWindow::destroyed,this,&Users::show);
           ui->nameEdit->setText("");
           ui->passwordEdit->setText("");

           return;
       }
   }
   QApplication::beep();
   errorMsg.setWindowIcon(QIcon(":/images/images/red_error_icon.png"));
   errorMsg.setWindowTitle("Error");
   errorMsg.setText("Incorrect name or password \nor user not found");
   errorMsg.show();
}

