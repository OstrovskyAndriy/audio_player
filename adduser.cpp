#include "adduser.h"
#include "ui_adduser.h"

AddUser::AddUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUser)
{
    ui->setupUi(this);

    connect(ui->addButton,&QPushButton::clicked,this,&AddUser::addUser);
    ui->addButton->setFocusPolicy(Qt::NoFocus);
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    error.setWindowIcon(QIcon(":/images/images/red_error_icon.png"));
    error.setWindowTitle("Error");

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
        openErrorMsg();
        return;
    }

    if(ui->nameEdit->text()==""||ui->passwordEdit->text()==""||ui->passwordEdit_2->text()==""){
        error.setText("Some field is empty");
        openErrorMsg();
    }

    else{
        if(dbManager->insertIntoUsers(ui->nameEdit->text(),ui->passwordEdit->text())){
            this->destroy();
            emit this->destroyed();
            return;
        }

        error.setText("User already present");
        openErrorMsg();
    }
}

void AddUser::openErrorMsg()
{
    QApplication::beep();
    error.exec();
}
