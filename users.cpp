#include "users.h"
#include "ui_users.h"

#include <QMovie>

Users::Users(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Users)
{
    ui->setupUi(this);
    this->setFixedSize(this->geometry().width(), this->geometry().height());

    dbManager = DBManager::getInstance();
    dbManager->connectToDataBase();

    ui->addUser->setFocusPolicy(Qt::NoFocus);
    ui->showPswrd->setFocusPolicy(Qt::NoFocus);

    hidePassword();

    // Встановлення GIF у QLabel
    QMovie *movie = new QMovie(":/images/images/cute_gif.gif");

    // Масштабування GIF до розміру loginLabel
    movie->setScaledSize(ui->labelForGif->size());
    ui->labelForGif->setMovie(movie);
    movie->start(); // Запускає анімацію
}

Users::~Users()
{
    delete ui;
}

void Users::on_addUser_clicked()
{
    addUser = new AddUser;
    addUser->show();
}

void Users::on_passwordEdit_returnPressed()
{
    const QString name = ui->nameEdit->text();
    const QString password = ui->passwordEdit->text();

    int userId = dbManager->getUserIdByLoginAndPassword(name, password);
    if (userId != -1) {
        qDebug() << "User authenticated. ID:" << userId;

        // Відкриваємо головне вікно
        playerWindow = new MainWindow(userId);
        this->hide();
        playerWindow->show();

        connect(playerWindow->window(), &MainWindow::destroyed, this, &Users::show);

        // Очищуємо поля після успішної авторизації
        ui->nameEdit->clear();
        ui->passwordEdit->clear();
    } else {
        QApplication::beep();
        errorMsg.setWindowIcon(QIcon(":/images/images/red_error_icon.png"));
        errorMsg.setWindowTitle("Error");
        errorMsg.setText("Дані введено не вірно або такого користувача не існує");
        errorMsg.show();
    }
}

void Users::showPassword()
{
    connect(ui->showPswrd, &QPushButton::clicked, this, &Users::hidePassword);
    disconnect(ui->showPswrd, &QPushButton::clicked, this, &Users::showPassword);
    ui->showPswrd->setIcon(QIcon(":/images/images/hide_password_icon.png"));

    ui->passwordEdit->setEchoMode(QLineEdit::Normal);
}

void Users::hidePassword()
{
    connect(ui->showPswrd, &QPushButton::clicked, this, &Users::showPassword);
    disconnect(ui->showPswrd, &QPushButton::clicked, this, &Users::hidePassword);
    ui->showPswrd->setIcon(QIcon(":/images/images/show_password_icon.png"));

    ui->passwordEdit->setEchoMode(QLineEdit::Password);
}


void Users::on_loginBtn_clicked()
{
    this->on_passwordEdit_returnPressed();
}

