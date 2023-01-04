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

