#include "adduser.h"
#include "ui_adduser.h"

#include <QMovie>

AddUser::AddUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUser)
{
    ui->setupUi(this);

    connect(ui->addButton,&QPushButton::clicked,this,&AddUser::addUser);
    connect(ui->showPswrd,&QPushButton::clicked,this,&AddUser::showPassword);

    ui->addButton->setFocusPolicy(Qt::NoFocus);
    ui->showPswrd->setFocusPolicy(Qt::NoFocus);
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    error.setWindowIcon(QIcon(":/images/images/red_error_icon.png"));
    error.setWindowTitle("Помилка");

    dbManager=DBManager::getInstance();
    //dbManager->connectToDataBase();

    this->hidePassword();

    QMovie *movie = new QMovie(":/images/images/nota.gif");

    // Масштабування GIF до розміру loginLabel
    movie->setScaledSize(ui->labelForGif->size());
    ui->labelForGif->setMovie(movie);
    movie->start();
}

AddUser::~AddUser()
{
    delete ui;
}

void AddUser::addUser()
{
    if(ui->passwordEdit->text()!=ui->passwordEdit_2->text()){
        error.setText("Паролі не збігаються");
        openErrorMsg();
        return;
    }

    if(ui->nameEdit->text()==""||ui->passwordEdit->text()==""||ui->passwordEdit_2->text()==""){
        error.setText("Заповніть всі поля");
        openErrorMsg();
    }

    else{
        if(dbManager->insertIntoUsers(ui->nameEdit->text(),ui->passwordEdit->text())){
            this->destroy();
            emit this->destroyed();
            return;
        }

        error.setText("Користувач вже присутній");
        openErrorMsg();
    }
}

void AddUser::openErrorMsg()
{
    QApplication::beep();
    error.exec();
}

void AddUser::showPassword()
{
    connect(ui->showPswrd,&QPushButton::clicked,this,&AddUser::hidePassword);
    disconnect(ui->showPswrd,&QPushButton::clicked,this,&AddUser::showPassword);
    ui->showPswrd->setIcon(QIcon(":/images/images/hide_password_icon.png"));

    ui->passwordEdit->setEchoMode(QLineEdit::Normal);
    ui->passwordEdit_2->setEchoMode(QLineEdit::Normal);
}

void AddUser::hidePassword()
{
    connect(ui->showPswrd,&QPushButton::clicked,this,&AddUser::showPassword);
    disconnect(ui->showPswrd,&QPushButton::clicked,this,&AddUser::hidePassword);
    ui->showPswrd->setIcon(QIcon(":/images/images/show_password_icon.png"));

    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit_2->setEchoMode(QLineEdit::Password);
}
