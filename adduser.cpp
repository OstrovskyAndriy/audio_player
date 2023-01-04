#include "adduser.h"
#include "ui_adduser.h"

AddUser::AddUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUser)
{
    ui->setupUi(this);

    connect(ui->addButton,&QPushButton::clicked,this,&AddUser::addUser);
    ui->addButton->setFocusPolicy(Qt::NoFocus);


    dbManager=DBManager::getInstance();
    //dbManager->connectToDataBase();
}

AddUser::~AddUser()
{
    delete ui;
}

void AddUser::addUser()
{
    if(ui->passwordEdit->text()!=ui->passwordEdit_2->text()){
        error.setText("Passwords don`t match");
        error.show();
        return;
    }

    if(ui->nameEdit->text()==""||ui->passwordEdit->text()==""||ui->passwordEdit_2->text()==""){
        error.setText("Some field is empty");
        error.show();
    }
    else{
        dbManager->insertIntoUsers(ui->nameEdit->text(),ui->passwordEdit->text());
        //this->hide();
        this->destroy();
        emit this->destroyed();
    }
}
