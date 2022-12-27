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
    /* Перед підключенням до бази даних виконуємо перевірку на її існування.
     * В залежності від результату виконуємо відкриття бази даних або її відновлення
     * */
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

/* Методи відновлення бази даних
 * */
bool DBManager::restoreDataBase()
{
    if(this->openDataBase()){
        if(!this->createMusicTable()){
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

bool DBManager::createMusicTable()
{

    QSqlQuery query;
    if(!query.exec("CREATE TABLE audioList ("
                   "music_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                   "path VARCHAR(100) NOT NULL,"
                   "song_name VARCHAR(100) NOT NULL);")){


        qDebug() << query.lastError().text();
        return false;
    } else
        return true;
}

bool DBManager::inserIntoTable(QString &songUrl,QString &song_name)
{
    QSqlQuery query;

    query.prepare("INSERT INTO audioList("
                       "path, "
                       "song_name)"
                       "VALUES(?,?);");

        query.addBindValue(songUrl);
        query.addBindValue(song_name);

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

