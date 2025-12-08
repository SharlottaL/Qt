#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include<QMediaPlaylist>
#include<QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
	void loadFileToPlaylist(const QString& filename);

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

private:
    Ui::MainWindow *ui;
	QMediaPlayer* m_player;
	QMediaPlaylist* m_playlist;
	QStandardItemModel* m_playlist_model;
	QMediaPlayer m_duration_player;
};
#endif // MAINWINDOW_H
