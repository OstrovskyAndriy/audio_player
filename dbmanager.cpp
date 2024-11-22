#include "dbmanager.h"

DBManager* DBManager::instance = nullptr;

DBManager::DBManager(){

}

DBManager* DBManager::getInstance()
{
    if(instance == nullptr){
        instance = new DBManager();
    }
    return instance;
}


void DBManager::connectToDataBase()
{

    if(QFile("audioDB.sqlite").exists()){
        this->openDataBase();
    } else {
        this->restoreDataBase();
    }
}

QSqlDatabase DBManager::getDB()
{
    return db;
}


bool DBManager::restoreDataBase()
{
    if(this->openDataBase()){
        if(!this->createTables()){
            return false;
        } else {
            return true;
        }
    } else {
        qDebug() << "Не вдалось відновити базу даних";
        return false;
    }
}


bool DBManager::openDataBase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("audioDB.sqlite");
    if(db.open()){
        return true;
    } else
        return false;
}


void DBManager::closeDataBase()
{
    db.close();
}

bool DBManager::createTables()
{
    QSqlQuery query;

    if(!query.exec("CREATE TABLE users("
                   "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                   "user_name VARCHAR(100) NOT NULL UNIQUE,"
                   "password VARCHAR(100) NOT NULL);"))
    {
        qDebug()<<"users error creating";
        qDebug() << query.lastError().text();
        return false;
    }

    if(!query.exec("CREATE TABLE audioList ("
                   "music_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                   "path VARCHAR(100) NOT NULL,"
                   "song_name VARCHAR(100) NOT NULL,"
                   "user_id INTEGER NOT NULL,"
                   "FOREIGN KEY(user_id) REFERENCES users(id));"))
    {
        qDebug()<<"audioList error creating";
        qDebug() << query.lastError().text();
        return false;
    }

        return true;
}

QSqlQueryModel* DBManager::getAudioListModel(int userId)
{
    QSqlQueryModel* model = new QSqlQueryModel();

    QString queryStr = QString("SELECT music_id, path, song_name FROM audioList WHERE user_id = %1;")
                       .arg(userId);
    model->setQuery(queryStr);

    if (model->lastError().isValid()) {
        qDebug() << "Error fetching audio list:" << model->lastError().text();
        delete model; // Видаляємо модель, якщо є помилка
        return nullptr;
    }

    // Встановлюємо заголовки стовпців українською
    model->setHeaderData(0, Qt::Horizontal, "ID Музики");
    model->setHeaderData(1, Qt::Horizontal, "Шлях");
    model->setHeaderData(2, Qt::Horizontal, "Назва Пісні");

    return model;
}


bool DBManager::inserIntoPlaylist(QString &songUrl,QString &song_name,int ID) const
{
    QSqlQuery query;

    query.prepare("INSERT INTO audioList("
                       "path, "
                       "song_name,"
                       "user_id)"
                       "VALUES(?,?,?);");

        query.addBindValue(songUrl);
        query.addBindValue(song_name);
        query.addBindValue(ID);

    if(!query.exec()){
        qDebug() << query.lastError().text();
        qDebug() << query.lastQuery();

        return false;
    } else
        return true;
}



int DBManager::getUserIdByLoginAndPassword(const QString& login, const QString& password) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE user_name = :login AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qDebug() << "Error fetching user ID:" << query.lastError().text();
        return -1; // Помилка виконання запиту
    }

    if (query.next()) {
        return query.value("id").toInt(); // Повертаємо знайдений ID
    }

    return -1; // Користувача не знайдено
}

bool DBManager::insertIntoUsers(QString name, QString password) const
{
    QSqlQuery query(db);

    query.prepare("INSERT INTO users("
                       "user_name, "
                       "password)"
                       "VALUES(?,?);");

        query.addBindValue(name);
        query.addBindValue(password);

    if(!query.exec()){
        qDebug() << query.lastError().text();
        qDebug() << query.lastQuery();

        return false;
    } else
        return true;
}



QString DBManager::getAudioTableName()
{
    return "audioList";
}

QString DBManager::getUsersTableName()
{
    return "users";
}

