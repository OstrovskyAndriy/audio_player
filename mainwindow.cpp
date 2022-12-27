#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableViewAudio->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableViewAudio->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setFixedSize(this->geometry().width(),this->geometry().height()-23);

    connect(ui->playAndStopSong, &QPushButton::clicked,this, &MainWindow::playMusic);
    connect(ui->muteButton,&QPushButton::clicked,this,&MainWindow::muteMusic);
    connect(ui->musicSlider, &QSlider::sliderMoved, this, &MainWindow::seek);

    connect(this,&MainWindow::nextClicked,this,&MainWindow::on_nextSong_clicked);

    dbManager=DBManager::getInstance();
    dbManager->connectToDataBase();

    model =new QSqlTableModel(this,dbManager->getDB());

    model->setTable(dbManager->getAudioTableName());

    vievOfTable();

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);

    connect(player,&QMediaPlayer::positionChanged,this,&MainWindow::onPositionChanged);
    connect(player,&QMediaPlayer::durationChanged,this,&MainWindow::onDurationChanged);

    ui->volumeSlider->setSliderPosition(50);
    ui->volumeSlider->setRange(0,100);
    ui->song_name->setText("");
    ui->songTime->setText("00:00/00:00");
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

    QString checkIsSongPresent;
    for(int i=0;i<ui->tableViewAudio->model()->rowCount();i++){
        checkIsSongPresent=ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(i,2)).toString();

        if(checkIsSongPresent==fileName){
            errorMsg.setText(fileName+"\nThis file already present");
            openErrorDiag();
            return;
        }
    }

    if(QDir("music").exists()){     //перевіряю чи є папка musiс, якщо є то копіюю файл
        QFile::copy(file,"./music/"+fileName);

        newFilePath=QFileInfo("./music/"+fileName).absoluteFilePath();

    }
    else{
        QDir().mkdir("music"); //інше створюю папку і копіюю файл
        QFile::copy(file,"./music/"+fileName);
        newFilePath=QFileInfo("./music/"+fileName).absoluteFilePath();
    }

    if(!dbManager->inserIntoTable(newFilePath,fileName)){
        errorMsg.setText("Error entering data");
        openErrorDiag();
    }

    vievOfTable();
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
    player->play();

    ui->playAndStopSong->setIcon(QIcon(":/images/images/pause_icon.png"));
    ui->playAndStopSong->setIconSize(QSize(25,25));

    connect(ui->playAndStopSong, &QPushButton::clicked, this, &MainWindow::stopMusic);
    disconnect(ui->playAndStopSong, &QPushButton::clicked, this, &MainWindow::playMusic);
}

void MainWindow::stopMusic()
{
    player->pause();

    ui->playAndStopSong->setIcon(QIcon(":/images/images/play_icon.png"));
    ui->playAndStopSong->setIconSize(QSize(25,25));

    disconnect(ui->playAndStopSong, &QPushButton::clicked, this, &MainWindow::stopMusic);
    connect(ui->playAndStopSong, &QPushButton::clicked, this, &MainWindow::playMusic);
}

void MainWindow::muteMusic()
{
    audioOutput->setMuted(true);

    ui->muteButton->setIcon(QIcon(":/images/images/mute_icon.png"));
    ui->muteButton->setIconSize(QSize(25,25));

    connect(ui->muteButton,&QPushButton::clicked,this,&MainWindow::unmuteMusic);
    disconnect(ui->muteButton,&QPushButton::clicked,this,&MainWindow::muteMusic);
    this->saveSliderPosition=ui->volumeSlider->sliderPosition();
    ui->volumeSlider->setSliderPosition(0);
}

void MainWindow::unmuteMusic()
{
    audioOutput->setMuted(false);
    ui->muteButton->setIcon(QIcon(":/images/images/unmute_icon.png"));
    ui->muteButton->setIconSize(QSize(25,25));

    connect(ui->muteButton,&QPushButton::clicked,this,&MainWindow::muteMusic);
    disconnect(ui->muteButton,&QPushButton::clicked,this,&MainWindow::unmuteMusic);
    ui->volumeSlider->setSliderPosition(this->saveSliderPosition);
}

void MainWindow::on_volumeSlider_sliderMoved()  // виконуєтсья лише коли регулятор пересувати
{
    qreal linearVolume =  QAudio::convertVolume(ui->volumeSlider->value() / qreal(100),
                                                QAudio::LogarithmicVolumeScale,
                                                QAudio::LinearVolumeScale);
    audioOutput->setVolume(linearVolume);

    if(ui->volumeSlider->sliderPosition()==1){
        this->unmuteMusic();
    }
}

void MainWindow::on_volumeSlider_valueChanged() // виконуєтсья лише коли клацнути по регулятору гучності
{
    qreal linearVolume =  QAudio::convertVolume(ui->volumeSlider->value() / qreal(100),
                                                QAudio::LogarithmicVolumeScale,
                                                QAudio::LinearVolumeScale);
    audioOutput->setVolume(linearVolume);
    if(ui->volumeSlider->sliderPosition()==10){
        saveSliderPosition=10;
        this->unmuteMusic();
    }
}

void MainWindow::on_tableViewAudio_doubleClicked(const QModelIndex &index)
{
    url = ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(index.row(),1)).toString();
    songName = ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(index.row(),2)).toString();
    songIndex = index.row();
    rowToDelete = songIndex;

    if(!QFile(url).exists()){
        ui->tableViewAudio->model()->removeRow(songIndex);
        vievOfTable();
        errorMsg.setText(songName+"\nNot found");
        openErrorDiag();
    }

    else
        this->playSong(url);
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
        songIndex=ui->tableViewAudio->model()->rowCount()-1; //якщо викликано слот на нульвій позиції пісні ставлю індекс на останню
    }

    rowToDelete=songIndex;

    url=ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(songIndex,1)).toString();
    songName =ui->tableViewAudio->model()->data(ui->tableViewAudio->model()->index(songIndex,2)).toString();

    this->playSong(url);
}

void MainWindow::on_offMusic_clicked()
{
    player->stop();
    this->stopMusic();
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
        ui->songTime->setText("00:00/00:00");
    }

    if(rowToDelete==0){
        rowToDelete=songIndex=ui->tableViewAudio->model()->rowCount()-1;
    }

    if(rowToDelete==ui->tableViewAudio->model()->rowCount()){
        rowToDelete=songIndex=-1;
    }
}

void MainWindow::seek(int mseconds)
{
    player->setPosition(mseconds);
}

void MainWindow::onDurationChanged(qint64 duration)
{

    ui->musicSlider->setRange(0, duration);
    currentSongDuration = duration;
}

void MainWindow::onPositionChanged(qint64 progress)
{
    qDebug()<<progress;
    qDebug()<<currentSongDuration;

    if(progress == currentSongDuration && progress != 0) {
        qDebug() << progress;
        qDebug() << currentSongDuration;
        player->stop();
        emit nextClicked();
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

        QTime time=QTime::fromMSecsSinceStartOfDay(player->duration());

        tStr = currentTime.toString(format) + " / " + totalTime.toString(time.toString("mm:ss"));
    }
    ui->songTime->setText(tStr);
}

void MainWindow::vievOfTable()
{
    model->setTable(dbManager->getAudioTableName());
    model->select();

    model->setHeaderData(2,Qt::Horizontal,QObject::tr("songs"));
    ui->tableViewAudio->verticalHeader()->setVisible(false);

    ui->tableViewAudio->setModel(model);
    ui->tableViewAudio->hideColumn(0);
    ui->tableViewAudio->hideColumn(1);
    ui->tableViewAudio->setColumnWidth(2,ui->tableViewAudio->width());
}

void MainWindow::openErrorDiag()
{
    errorMsg.setWindowIcon(QIcon(":/images/images/red_error_icon.png"));
    errorMsg.setWindowTitle("Error");
    QApplication::beep();
    errorMsg.exec();
}

void MainWindow::on_closeWindow_clicked()
{
    QApplication::exit();
}
