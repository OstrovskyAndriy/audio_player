#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSqlTableModel> // для відображення таблиць
#include <QTableView>
#include <QStringList>
#include <QFileDialog>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSlider>
#include <QFile>
#include <QDir>
#include <QTime>
#include <QSqlQueryModel>

#include "dbmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const int userID, QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void nextClicked();
private slots:
    void on_Add_clicked();
    void on_tableViewAudio_doubleClicked(const QModelIndex &index);
    void playMusic();
    void stopMusic();
    void muteMusic();
    void unmuteMusic();
    void playSong(QString songUrl);
    void on_volumeSlider_sliderMoved();
    void on_volumeSlider_valueChanged();
    void on_nextSong_clicked();
    void on_prevSong_clicked();
    void on_offMusic_clicked();
    void on_closeWindow_clicked();
    void on_tableViewAudio_clicked(const QModelIndex &index);
    void on_deleteButton_clicked();
    void vievOfTable();
    void onDurationChanged(qint64 duration);
    void onPositionChanged(qint64 progress);
    void seek(int mseconds);

    void openErrorMsg();

    void on_goToUsers_clicked();


private:
    Ui::MainWindow *ui;
    DBManager *dbManager;

    QSqlQueryModel * model;

    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QString url;
    QString songName;
    QString songNameToDelete;

    QMessageBox errorMsg;

    int userID;
    int songIndex;
    int rowToDelete;
    void updateDurationInfo(qint64 currentInfo);
    qint64 currentSongDuration;
    int saveSliderPosition;
    QMovie *movie;
};
#endif // MAINWINDOW_H
