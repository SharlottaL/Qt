#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
	void on_pushButton_Add_clicked();

	void on_pushButton_Play_clicked();

	void on_pushButton_Pause_clicked();

	void on_pushButton_Stop_clicked();

	void on_Volume_sliderMoved(int position);

	void positionChanged(qint64 position);
	void durationChanged(qint64 duration);

private:
    Ui::MainWindow *ui;
	QMediaPlayer* m_player;
};
#endif // MAINWINDOW_H
