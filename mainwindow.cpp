#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include<QFileDialog>
#include<QTime>
#include <QDir>

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
//ui->pushButton_Shuffle->setIcon(stule()->standardIcon(QStyle::SP_Media));
//ui->pushButton_Loop->setIcon(style()->standardIcon(QStyle::))

	m_player = new QMediaPlayer();
	//https://doc.qt.io/archives/qt-5.15/qmediaplayer.html
	connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
	connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);
	connect(this->ui->horizontalSliderTime, &QSlider::sliderMoved,this, &MainWindow::on_horizontalSliderTime_sliderMoved);
	///Playlist init:
	m_playlist_model=new QStandardItemModel(this);
	initPlaylist();




	m_playlist = new QMediaPlaylist(m_player);
	m_player->setPlaylist(m_playlist);


	ui->Volume->setRange(0,100);
	m_player->setVolume(30);
	ui->label_Volume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
	ui->Volume->setValue(m_player->volume());
	connect(this->ui->pushButton_Prev, &QPushButton::clicked, this-> m_playlist,&QMediaPlaylist::previous);
	connect(this->ui->pushButton_Next, &QPushButton::clicked, this-> m_playlist,&QMediaPlaylist::next);
	connect(this->m_playlist, &QMediaPlaylist::currentIndexChanged, this->ui->tableViewPlaylist,&QTableView::selectRow);
	connect(this->ui->tableViewPlaylist,&QTableView::doubleClicked,
			[this](const QModelIndex& index){m_playlist->setCurrentIndex(index.row()); this ->m_player->play();}
	);
	connect(this->m_player, &QMediaPlayer::mediaChanged,
			[this](const QMediaContent& media)
	{
		this->ui->label_File ->setText(media.canonicalUrl().toString());
		this->setWindowTitle(this->ui->label_File->text().split('/').last());
	});
	shuffle = false;
	loop = false;
	//connect(this->ui->pushButton_Clr,&QPushButton::clicked, this->m_playlist, &QMediaPlaylist::clear);
	//connect(this->ui->pushButton_Clr,&QPushButton::clicked, this->m_playlist_model, &QStandardItemModel::clear);
	loadPlaylist("muzlo.m3u");
}
MainWindow::~MainWindow()
{
	savePlaylist("muzlo.m3u");
	delete m_playlist;
	delete m_playlist_model;
	delete m_player;
	delete ui;
}

void MainWindow::initPlaylist()
{
	this->ui->tableViewPlaylist->setModel(m_playlist_model);
	m_playlist_model->setHorizontalHeaderLabels(QStringList()<<"Audio track"<<"File path"<<"Duration");
	this->ui->tableViewPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->ui->tableViewPlaylist->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->ui->tableViewPlaylist->hideColumn(1);
	int duration_width=64;
	this->ui->tableViewPlaylist->setColumnWidth(3,duration_width);
	this->ui->tableViewPlaylist->setColumnWidth(0,this->ui->tableViewPlaylist->width()-duration_width*1.7);
}

bool isAudioExtension(const QString& filename)
{
	if(filename.split('.').last() == "mp3")return true;
	if(filename.split('.').last() == "flac")return true;
	if(filename.split('.').last() == "flacc")return true;
	return false;
}

void MainWindow::loadFileToPlaylist(const QString& filename)
{
	if(!isAudioExtension(filename))return;
	m_playlist->addMedia(QUrl(filename));
	QList<QStandardItem*> items;
	items.append(new QStandardItem(QDir(filename).dirName()));
	items.append(new QStandardItem(filename));
//	QMediaPlayer player;
//	player.setMedia(QUrl(filename));
//	player.play();

//	items.append(new QStandardItem(QTime::fromMSecsSinceStartOfDay(player.duration()).toString("mm:ss")));
//	player.pause();
	m_playlist_model->appendRow(items);
}

void MainWindow::savePlaylist(const QString &filename)
{
	QString format = filename.split('.').last();
	QUrl url = QUrl::fromLocalFile(filename);
	bool result = m_playlist->save(url, format.toStdString().c_str());
}

void MainWindow::loadPlaylist(const QString &filename)
{
	m_playlist->load(QUrl::fromLocalFile(filename));
	int n = m_playlist->mediaCount();
	for(int i=0;i<n; i++)
	{
	   QList<QStandardItem*> item;
	   item.append(new QStandardItem(m_playlist->media(i).canonicalUrl().fileName()));
	   item.append(new QStandardItem(m_playlist->media(i).canonicalUrl().path()));
	   m_playlist_model->appendRow(item);
	}
}

QVector<QString> MainWindow::loadPlaylistToArray(const QString &filename)
{
	QFile file(filename);
	file.open(QIODevice::ReadOnly);
	QList<QString> lines;
	while(!file.atEnd())
	{
		QByteArray line = file.readLine();
		lines.append(line);
	}
	file.close();
	return lines.toVector();
}

void MainWindow::on_pushButton_Add_clicked()
{
//	QString file = QFileDialog::getOpenFileName
//		   (
//				this,
//				"Open file",
//				"C:\muz",
//				"Audio files (*.mp3 *.flac);; MP-3 (*.mp3);; Flac(*.flac)"
//		   );
//	ui->label_File->setText(QString("File:").append(file));
//	this->m_player->setMedia(QUrl(file));

	QStringList files = QFileDialog::getOpenFileNames
		(
			this,
			"Open file",
			"D:\\muzlo",
			"Audio files (*.mp3 *.flac *.flacc);; MP-3 (*.mp3);; Flac(*.flac *.flacc)"
		);

	for(QString file:files)
	{
	  loadFileToPlaylist(file);
	}

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


void MainWindow::on_pushButton_Shuffle_clicked()
{
	shuffle=!shuffle;
	this->ui->pushButton_Shuffle->setCheckable(true);
	this->m_playlist->setPlaybackMode(shuffle ? QMediaPlaylist::PlaybackMode::Random: QMediaPlaylist::PlaybackMode::Sequential);
	this->ui->pushButton_Shuffle->setChecked(shuffle);
}


void MainWindow::on_pushButton_Shuffle_2_clicked()
{
	loop = !loop;
	this->ui->pushButton_Shuffle_2->setCheckable(true);
	this->m_playlist->setPlaybackMode(loop ? QMediaPlaylist::PlaybackMode::Loop : QMediaPlaylist::PlaybackMode::Sequential);
	this->ui->pushButton_Shuffle_2->setChecked(loop);
}


void MainWindow::on_pushButton_Del_clicked()
{
//QItemSelectionModel* selection = ui-> tableViewPlaylist->selectionModel();
//QModelIndexList indexes = selection -> selectedRows();
//for(QModelIndex i : indexes)
//{
//	m_playlist_model->removeRows(i.row(), 1);
//	m_playlist->removeMedia(i.row());
	QItemSelectionModel* selection = nullptr;
	do
	{
		selection = ui->tableViewPlaylist->selectionModel();
		QModelIndexList indexes = selection->selectedRows();
		if(selection->selectedRows().count()>0)
		{
			m_playlist_model->removeRow(indexes.first().row());
			m_playlist->removeMedia(indexes.first().row());
		}
	} while(selection->selectedRows().count());
}


void MainWindow::on_pushButton_Clr_clicked()
{
	m_playlist->clear();
	m_playlist_model->clear();
	initPlaylist();
}

void MainWindow::traverseDirectories(const QString& dirname)
{
	QDir dir = QDir(dirname);
	for(QFileInfo file : dir.entryInfoList())
	{
		QString filename = file.fileName();
		if(file.fileName() == "." || file.fileName() == "..")continue;
		if(file.isFile())
		{
			QString filename = dir.filePath(file.fileName());
			loadFileToPlaylist(filename);
		}
		if(file.isDir())
		{
			traverseDirectories(dir.filePath(file.fileName()));
		}
	}
}

void MainWindow::on_pushButton_Dir_clicked()
{
	QString dirname = QFileDialog::getExistingDirectory
	(
		this,
		"Add directory",
		"D:\\muzlo",
		QFileDialog::ShowDirsOnly
	);
	traverseDirectories(dirname);
}

