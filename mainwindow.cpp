#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableViewAudio->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableViewAudio->setEditTriggers(QAbstractItemView::NoEditTriggers);


    connect(ui->playAndStopSong, &QPushButton::clicked,this, &MainWindow::playMusic);
    connect(ui->musicSlider, &QSlider::sliderMoved, this, &MainWindow::seek);



    dbManager.createAudioDB();

    model =new QSqlTableModel(this,dbManager.returnAudioDB());

    model->setTable(dbManager.getAudioTableName());


    vievOfTable();

    //qDebug("create table");


    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);

    connect(player,&QMediaPlayer::positionChanged,this,&MainWindow::onPositionChanged);
    connect(player,&QMediaPlayer::durationChanged,this,&MainWindow::onDurationChanged);

    //регулювання гучності
    ui->volumeSlider->setSliderPosition(50);
    ui->volumeSlider->setRange(0,100);
    ui->songTime->setText("");

}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete player;
}


void MainWindow::on_Add_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Open files"),QString(),tr("Audio Files (*.mp3)")); //шлях до файлу

    if(file==""){
        return;
    }

    QFileInfo info(file);
    QString fileName = info.fileName(); //отримати назву файлу
    QString newFilePath;

    ui->song_name->setText(fileName);


    if(QDir("music").exists()){     //перевіряю чи є папка musiс, якщо є то копіюю файл
        QFile::copy(file,"./music/"+fileName);

        newFilePath=QFileInfo("./music/"+fileName).absoluteFilePath();

    }
    else{
        QDir().mkdir("music"); //інше створюю папку і копіюю файл
        QFile::copy(file,"./music/"+fileName);

    }


    if(!dbManager.insert(newFilePath,fileName)){
        errorMsg.setText("Error entering data");
        errorMsg.exec();
    }


    vievOfTable();

    songIndex=ui->tableViewAudio->model()->columnCount();
    rowToDelete=songIndex;
}


void MainWindow::playSong(QString songUrl)
{
    player->stop();
    ui->song_name->setText(songName);
    player->setSource(QUrl::fromLocalFile(songUrl));

    this->playMusic();
}

void MainWindow::playMusic()
{  
    ui->playAndStopSong->setText("Pause");
    player->play();

    connect(ui->playAndStopSong, &QPushButton::clicked, this, &MainWindow::stopMusic);
    disconnect(ui->playAndStopSong, &QPushButton::clicked, this, &MainWindow::playMusic);
}


void MainWindow::stopMusic()
{
    ui->playAndStopSong->setText("Play");
    player->pause();

    disconnect(ui->playAndStopSong, &QPushButton::clicked, this, &MainWindow::stopMusic);
    connect(ui->playAndStopSong, &QPushButton::clicked, this, &MainWindow::playMusic);
}


void MainWindow::on_volumeSlider_sliderMoved()  // виконуєтсья лише коли регулятор пересувати
{
    qreal linearVolume =  QAudio::convertVolume(ui->volumeSlider->value() / qreal(100),
                                                QAudio::LogarithmicVolumeScale,
                                                QAudio::LinearVolumeScale);
    audioOutput->setVolume(linearVolume);
}

void MainWindow::on_volumeSlider_valueChanged() // виконуєтсья лише коли клацнути по регулятору гучності
{
    qreal linearVolume =  QAudio::convertVolume(ui->volumeSlider->value() / qreal(100),
                                                QAudio::LogarithmicVolumeScale,
                                                QAudio::LinearVolumeScale);
    audioOutput->setVolume(linearVolume);
}

void MainWindow::on_tableViewAudio_doubleClicked(const QModelIndex &index)
{
    url = ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(index.row(),1)).toString();
    songName = ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(index.row(),2)).toString();
    songIndex = index.row();
    rowToDelete = songIndex;

    //перевірка чи файл дійсний
    //якщо не дійсний то видалити з бази даних
    if(!QFile(url).exists()){
        ui->tableViewAudio->model()->removeRow(songIndex);
        vievOfTable();

        //Вивести про це помилку
        //QMessageBox errorMsg;
        errorMsg.setText(songName+" not found");
        errorMsg.exec();
    }

    else{
        //в іншому випадку включити музику
        this->playSong(url);

    }
}


void MainWindow::on_nextSong_clicked()
{
    songIndex++;

    if(songIndex==ui->tableViewAudio->model()->rowCount()){
        songIndex=0;
    }

    rowToDelete=songIndex;

    url=ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(songIndex,1)).toString();
    songName = ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(songIndex,2)).toString();

    this->playSong(url);
}


void MainWindow::on_prevSong_clicked()
{
    songIndex--;
    if(songIndex==-1){
        songIndex=ui->tableViewAudio->model()->rowCount()-1;
    }

    rowToDelete=songIndex;

    url=ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(songIndex,1)).toString();
    songName =ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(songIndex,2)).toString();

    this->playSong(url);
}


void MainWindow::on_offMusic_clicked()
{
    player->stop();
}



void MainWindow::on_closeWindow_clicked()
{
    QApplication::exit();
}


void MainWindow::on_tableViewAudio_clicked(const QModelIndex &index)
{
    rowToDelete=index.row();
}


void MainWindow::on_deleteButton_clicked()
{
    ui->tableViewAudio->model()->removeRow(rowToDelete);
    vievOfTable();

    if(songIndex==rowToDelete){
        player->stop();
    }
    qDebug()<<rowToDelete;

    if(rowToDelete==0){
        rowToDelete=songIndex=ui->tableViewAudio->model()->rowCount()-1;
    }

    if(rowToDelete==ui->tableViewAudio->model()->rowCount()){
        rowToDelete=songIndex=-1;
    }
}

void MainWindow::vievOfTable()
{
    model->setTable(dbManager.getAudioTableName());
    model->select();
    ui->tableViewAudio->setModel(model);
    ui->tableViewAudio->hideColumn(0);
    ui->tableViewAudio->hideColumn(1);
    ui->tableViewAudio->setColumnWidth(2,ui->tableViewAudio->width());
}


void MainWindow::seek(int mseconds)
{
    player->setPosition(mseconds);
}

void MainWindow::setSliderPosition(qint64 position)
{
    qDebug()<< position;
}


void MainWindow::onDurationChanged(qint64 duration)
{

    ui->musicSlider->setRange(0, duration);
    currentSongDuration = duration;
}

void MainWindow::onPositionChanged(qint64 progress)
{
    if(progress == currentSongDuration && progress != 0) {
        qDebug() << progress;
        qDebug() << currentSongDuration;
        //player->stop();
        this->on_nextSong_clicked();
    }
    else {
        if (!ui->musicSlider->isSliderDown())
            ui->musicSlider->setValue(progress);
        updateDurationInfo(progress / 1000);
    }
}

void MainWindow::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    qint64 duration = currentSongDuration / 1000;
    if (currentInfo || duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
                          currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((duration / 3600) % 60, (duration / 60) % 60,
                        duration % 60, (duration * 1000) % 1000);
        QString format = "mm:ss";
        //if (m_duration > 3600)
        //  format = "hh:mm:ss";

        QTime time=QTime::fromMSecsSinceStartOfDay(player->duration());
        //        //qDebug()<<time.toString("mm:ss");

        tStr = currentTime.toString(format) + " / " + totalTime.toString(time.toString("mm:ss"));
    }
    ui->songTime->setText(tStr);
}
