#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include<QFileDialog>
#include<QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	ui->pushButton_Add->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));
	ui->pushButton_Prev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
	ui->pushButton_Next->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	ui->pushButton_Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui->pushButton_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	ui->pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui->pushButton_Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));


	m_player = new QMediaPlayer();
	//https://doc.qt.io/archives/qt-5.15/qmediaplayer.html
	connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
	connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);
	connect(this->ui->horizontalSliderTime, &QSlider::sliderMoved,this, &MainWindow::on_horizontalSliderTime_sliderMoved);
	ui->Volume->setRange(0,100);
	m_player->setVolume(30);
	ui->label_Volume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
	ui->Volume->setValue(m_player->volume());
}

MainWindow::~MainWindow()
{
	delete m_player;
    delete ui;
}

void MainWindow::on_pushButton_Add_clicked()
{
	QString file = QFileDialog::getOpenFileName
		   (
				this,
				"Open file",
				"",
				"Audio files (*.mp3 *.flac);; MP-3 (*.mp3);; Flac(*.flac)"
		   );
	ui->label_File->setText(QString("File:").append(file));
	this->m_player->setMedia(QUrl(file));
}

void MainWindow::on_pushButton_Play_clicked()
{
	this->m_player->play();
}

void MainWindow::on_pushButton_Pause_clicked()
{
	m_player->state()==QMediaPlayer::State::PausedState ? m_player->play() : this->m_player->pause();
}

void MainWindow::on_pushButton_Stop_clicked()
{
	this->m_player->stop();
}

void MainWindow::on_positionChanged(qint64 position)
{
	this->ui->label_Position->setText(QString(QTime::fromMSecsSinceStartOfDay(position).toString("hh:mm:ss")));
	this->ui->horizontalSliderTime->setValue(position);
}
void MainWindow::on_durationChanged(qint64 duration)
{
	this->ui->label_Duration->setText(QTime::fromMSecsSinceStartOfDay(duration).toString("hh:mm:ss"));
	this->ui->horizontalSliderTime->setRange(0, duration);
}

void MainWindow::on_pushButton_Mute_clicked()
{
	m_player->setMuted(!m_player->isMuted());
	ui->pushButton_Mute->setIcon(style()->standardIcon(m_player->isMuted()?QStyle::SP_MediaVolumeMuted:QStyle::SP_MediaVolume));
}


void MainWindow::on_Volume_valueChanged(int value)
{
	m_player->setVolume(value);
	ui->label_Volume->setText(QString("Volume:").append(QString::number(value)));
}


void MainWindow::on_horizontalSliderTime_valueChanged(qint64 value)
{
	m_player->setPosition(value);
}


void MainWindow::on_horizontalSliderTime_sliderMoved(qint64 position)
{
	this->m_player->setPosition(position);
}

