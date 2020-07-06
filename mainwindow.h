#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cardreader.h"
#include <QTimer>
#include <QFileSystemWatcher>
#include <QLabel>

#define NUM_OF_KEYS 8
//8 keys

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QFileSystemWatcher watcher;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void call_pin();

public slots:
    void processNewCardFromDb();


private slots:
    void startupViews();
    void connectDB();
    void start();
    void update_time();
    void unlockBox();
    void startforenter(QString id);
    void AfterLogin(QString employee_id);
    void openGetGive();
    void updateMain();
    void updateGet();
    void updateGive();
    void get_key(QString key);
    void give_key(QString key);
    void message(QString text, int lockNo);
    void returnmain();


    void on_get_key_clicked();

    void on_give_key_clicked();

    void on_get_key1_clicked();

    void on_give_key2_clicked();

    void on_give_key3_clicked();

    void on_give_key4_clicked();

    void on_give_key5_clicked();

    void on_give_key6_clicked();

    void on_give_key7_clicked();

    void on_give_key8_clicked();

    void on_get_key2_clicked();

    void on_get_key3_clicked();

    void on_get_key4_clicked();

    void on_get_key5_clicked();

    void on_get_key6_clicked();

    void on_get_key7_clicked();

    void on_get_key8_clicked();
    //key_9_ckilcked
    void on_get_key9_clicked();

    void on_give_key9_clicked();

    void on_give_key1__clicked();

    void on_give_cancel_clicked();

    void on_get_cancel_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    CardReader cardReader;
    QTimer *timer;
    QTimer *pinDelay;
    QTimer *refresh;

    QLabel *keys[NUM_OF_KEYS];
    QString keyTexts[NUM_OF_KEYS];
};

#endif // MAINWINDOW_H
