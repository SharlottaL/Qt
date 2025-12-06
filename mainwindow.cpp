#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include<QFileDialog>

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
	connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
	connect(m_player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
	ui->Volume->setRange(0,100);
	m_player->setVolume(50);
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
	this->m_player->pause();
}


void MainWindow::on_pushButton_Stop_clicked()
{
	this->m_player->stop();
}


void MainWindow::on_Volume_sliderMoved(int position)
{
	this->m_player->setVolume(position);
	ui->label_Volume->setText(QString::number(position));
}

void MainWindow::positionChanged(qint64 position)
{
	ui->horizontalSliderTime->setValue(position);
}
void MainWindow::durationChanged(qint64 duration)
{
	ui->horizontalSliderTime->setRange(0, duration);
}


