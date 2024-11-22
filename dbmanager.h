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
    bool createTables();

public:
    static DBManager* getInstance();

    void connectToDataBase();
    QSqlDatabase getDB();
    bool inserIntoPlaylist(QString &songUrl,QString &song_name,int ID) const;
    bool insertIntoUsers(QString name,QString password) const;

    QString getAudioTableName();
    QString getUsersTableName();

    int getUserIdByLoginAndPassword(const QString &login, const QString &password);
    QSqlQueryModel *getAudioListModel(int userId);
};

#endif // DBMANAGER_H
