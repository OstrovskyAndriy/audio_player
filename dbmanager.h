#ifndef DBMANAGER_H
#define DBMANAGER_H


#include <QSqlDatabase> // бібліотека бази даних
#include <QDebug> //бібліотека для відладки програми
#include <QSqlQuery> // бібліотека для запиту до баз даних
#include <QtSql>

class DBManager
{
    QSqlDatabase db;
    QSqlQuery *query;

    static DBManager* instance;

    DBManager();

    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createMusicTable();

public:
    static DBManager* getInstance();

    void connectToDataBase();
    QSqlDatabase getDB();
    bool inserIntoTable(QString &songUrl,QString &song_name);

    QString getAudioTableName();

};

#endif // DBMANAGER_H
