#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QFileDialog>
#include <QTime>
#include <QDir>
#include <QDebug>
#include <QAudioProbe>
#include <QAudioBuffer>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
	, ui(new Ui::MainWindow)
	   , m_equalizerActive(false)
	   , m_equalizerTimer(nullptr)

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
	ui->frame->setMinimumHeight(150);
		ui->frame->setMaximumHeight(200);
		ui->frame->setStyleSheet("background-color: #1e1e2e; border: 2px solid #2d2d44;");

		ui->frame->installEventFilter(this);
		m_equalizerLevels.resize(10);
		for (int i = 0; i < 10; ++i) {
			m_equalizerLevels[i] = 0;
		}

		m_equalizerTimer = new QTimer(this);
		connect(m_equalizerTimer, &QTimer::timeout, this, &MainWindow::updateEqualizer);
		m_equalizerTimer->start(50);
	loadPlaylist("muzlo.m3u");

   }
void MainWindow::drawEqualizer(QPainter &painter)
{
	painter.setRenderHint(QPainter::Antialiasing);

	int width = ui->frame->width();
	int height = ui->frame->height();

	QLinearGradient bgGradient(0, 0, 0, height);
	bgGradient.setColorAt(0, QColor(40, 40, 60));
	bgGradient.setColorAt(1, QColor(20, 20, 40));
	painter.fillRect(0, 0, width, height, bgGradient);

	painter.setPen(QPen(QColor(50, 50, 70, 100), 1));
	for (int i = 1; i < 5; i++) {
		int y = height * i / 5;
		painter.drawLine(0, y, width, y);
	}

	int barCount = m_equalizerLevels.size();
	if (barCount > 0) {
		int barWidth = width / barCount - 3;

		for (int i = 0; i < barCount; ++i) {
			int barHeight = qMax(5, m_equalizerLevels[i] * height / 100);
			barHeight = qMin(height - 10, barHeight);

			int x = i * (barWidth + 3) + 3;
			int y = height - barHeight;

			QLinearGradient barGradient(x, y, x, y + barHeight);

			if (m_player->state() == QMediaPlayer::PlayingState && m_equalizerActive) {
				barGradient.setColorAt(0, QColor(0, 200, 255));
				barGradient.setColorAt(0.7, QColor(0, 150, 220));
				barGradient.setColorAt(1, QColor(0, 100, 180));
			} else {
				barGradient.setColorAt(0, QColor(100, 100, 150));
				barGradient.setColorAt(1, QColor(70, 70, 100));
			}

			painter.setBrush(barGradient);
			painter.setPen(Qt::NoPen);
			painter.drawRect(x, y, barWidth, barHeight);

			painter.setPen(QPen(QColor(255, 255, 255, 30), 1));
			painter.setBrush(Qt::NoBrush);
			painter.drawRect(x, y, barWidth, barHeight);
		}
	}
}
   void MainWindow::updateEqualizer()
   {
	   static int step = 0;
	   for (int i = 0; i < m_equalizerLevels.size(); ++i) {
		   double value = 0;

		   if (m_player->state() == QMediaPlayer::PlayingState && m_equalizerActive) {
			   value = 20 + 50 * sin(step * 0.08 + i * 0.7);

			   if (i < 3)
				   value = 30 + 40 * sin(step * 0.05 + i * 0.3);

			   else if (i > 7)
				   value = 15 + 30 * fabs(sin(step * 0.15 + i * 1.2));

			   if (rand() % 100 > 85)
				   value += 15 + rand() % 35;
		   } else
		   {
			   value = qMax(0, m_equalizerLevels[i] - 8);
			   if (rand() % 100 > 90)
				   value += 5 + rand() % 15;
		   }

		   m_equalizerLevels[i] = 0.7 * m_equalizerLevels[i] + 0.3 * value;
	   }
	   step++;

	   ui->frame->update();
   }

   bool MainWindow::eventFilter(QObject *watched, QEvent *event)
   {
	   if (watched == ui->frame && event->type() == QEvent::Paint) {
		   QPaintEvent *paintEvent = static_cast<QPaintEvent*>(event);
		   QPainter painter(ui->frame);
		   drawEqualizer(painter);
		   return true;
	   }
	   return QMainWindow::eventFilter(watched, event);
   }
MainWindow::~MainWindow()
{
	if (m_equalizerTimer) {
		   m_equalizerTimer->stop();
		   delete m_equalizerTimer;
	   }
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

bool MainWindow::isAudioExtension(const QString& filename)
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
	 m_equalizerActive = true;
}

void MainWindow::on_pushButton_Pause_clicked()
{
	if (m_player->state() == QMediaPlayer::PlayingState) {
		   m_player->pause();
		   m_equalizerActive = false;
	   } else {
		   m_player->play();
		   m_equalizerActive = true;
	   }
}

void MainWindow::on_pushButton_Stop_clicked()
{
	m_player->stop();
	   m_equalizerActive = false;
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

