#include "dbmanager.h"

DBManager::DBManager()
{

}

DBManager::~DBManager()
{
    delete query;
}

void DBManager::createAudioDB()
{

    db=QSqlDatabase::addDatabase("QSQLITE"); // драйвер sqlite
    db.setDatabaseName("./audioDB.sqlite"); // добавити базу даних в папку проекту

    if(db.open()){          // перевірка чи відкриється база даних
        qDebug("open");     // якщо відкривається в консоль виводим open, якщо ні відповідно no open
    }
    else{
        qDebug("no open");
        qDebug() << QSqlDatabase::drivers();
    }


    QString queryToCreateTable = "CREATE TABLE audioList ("
                                 "music_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                                 "path VARCHAR(100) NOT NULL,"
                                 "song_name VARCHAR(100) NOT NULL); ";
    query = new QSqlQuery;

    if(!query->exec(queryToCreateTable)){
        qDebug()<<"error creatig table";
    }

    qDebug("create table");

}

QString DBManager::getAudioTableName()
{
    return "audioList";
}

QSqlDatabase DBManager::returnAudioDB()
{
    return db;
}


bool DBManager::insert(QString &songUrl, QString &song_name) const
{
    query->prepare("INSERT INTO audioList("
                   "path, "
                   "song_name)"
                   "VALUES(?,?);");

    query->addBindValue(songUrl);
    query->addBindValue(song_name);

    return query->exec();

}



