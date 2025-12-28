#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QAudioProbe>
#include <QAudioBuffer>
#include <QTimer>
#include <QDockWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
	void initPlaylist();
	void loadFileToPlaylist(const QString& filename);
	void savePlaylist(const QString& filename);
	void loadPlaylist(const QString& filename);
	QVector<QString> loadPlaylistToArray(const QString& filename);
	void setupEqualizer();
	bool isAudioExtension(const QString& filename);
void drawEqualizer(QPainter &painter);
void onFramePaint();
bool eventFilter(QObject *watched, QEvent *event);
private slots:
	void on_pushButton_Add_clicked();

	void on_pushButton_Play_clicked();

	void on_pushButton_Pause_clicked();

	void on_pushButton_Stop_clicked();

	//void on_Volume_sliderMoved(int position);

	void on_positionChanged(qint64 position);
	void on_durationChanged(qint64 duration);

	void on_pushButton_Mute_clicked();

	void on_Volume_valueChanged(int value);

	void on_horizontalSliderTime_valueChanged(qint64 value);

	void on_horizontalSliderTime_sliderMoved(qint64 position);

	void on_pushButton_Shuffle_clicked();

	void on_pushButton_Shuffle_2_clicked();

	void on_pushButton_Del_clicked();

	void on_pushButton_Clr_clicked();


	void on_pushButton_Dir_clicked();
	void updateEqualizer();




private:
	void traverseDirectories(const QString& dirname);
    Ui::MainWindow *ui;
	QMediaPlayer* m_player;
	QMediaPlaylist* m_playlist;
	QStandardItemModel* m_playlist_model;
	QMediaPlayer m_duration_player;

	bool shuffle;
	bool loop;

	QCustomPlot *m_plot;
	   QTimer *m_equalizerTimer;
	   QVector<double> m_equalizerData;
	   QVector<double> m_frequencyBands;

	   QWidget *m_equalizerWidget;
		  bool m_equalizerActive;
		  QVector<int> m_equalizerLevels;

};
#endif // MAINWINDOW_H
