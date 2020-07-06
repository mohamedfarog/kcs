#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wiringPi.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql/QtSql>
#include <iostream>
#include <QDebug>
#include <QDateTime>
#include <QObject>
#include <sstream>
#include <QMovie>
#include <QProcess>

//keys = i3,imiev,vitz,meetinng,cargo,sys,vacum
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
QSqlQuery qry;
QString name,rfid,employee_id,card_id,id;
bool i3,imiev,vitz,cargo_door,c1f1,sys,vacum,vitz9;
bool login = false;
int lockNo;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    wiringPiSetup();
    pinMode(21,OUTPUT);
    pinMode(22,OUTPUT);
    pinMode(23,OUTPUT);
    pinMode(24,OUTPUT);
    pinMode(25,OUTPUT);
    pinMode(0,OUTPUT);
    pinMode(2,OUTPUT);
    pinMode(3,OUTPUT);
    pinMode(7,OUTPUT);

    digitalWrite(21,HIGH);
    digitalWrite(22,HIGH);
    digitalWrite(23,HIGH);
    digitalWrite(24,HIGH);
    digitalWrite(25,HIGH);
    digitalWrite(0,HIGH);
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    digitalWrite(7,HIGH);

    startupViews();
    connectDB();
    start();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(update_time()));
    timer->start(1000);

    connect(&cardReader, SIGNAL(newCardAvailableOnDb()),this,SLOT(processNewCardFromDb()));
    cardReader.run();
    pinDelay = new QTimer(this);
    connect(pinDelay, SIGNAL(timeout()),this,SLOT(unlockBox()));

    refresh = new QTimer(this);
    connect(refresh, SIGNAL(timeout()),this,SLOT(updateMain()));

    refresh->start(5000);


    // Key definitions
    keys[0] = ui->main_key1_status;
    keys[1] = ui->main_key2_status;
    keys[2] = ui->main_key3_status;
    keys[3] = ui->main_key4_status;
    keys[4] = ui->main_key5_status;
    keys[5] = ui->main_key6_status;
    keys[6] = ui->main_key7_status;
    keys[7] = ui->main_key9_status;
    keyTexts[0] = "PZ067";
    keyTexts[1] = "LZ013";
    keyTexts[2] = "ME416";
    keyTexts[3] = "";
    keyTexts[4] = "";
    keyTexts[5] = "";
    keyTexts[6] = "";
    keyTexts[7] = "VITZ4";

    updateMain();
}



void MainWindow::unlockBox(){
    qDebug() << "workkkkkk";

    digitalWrite(21,HIGH);
    digitalWrite(22,HIGH);
    digitalWrite(23,HIGH);
    digitalWrite(24,HIGH);
    digitalWrite(25,HIGH);
    digitalWrite(0,HIGH);
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    digitalWrite(7,HIGH);

    pinDelay->stop();

    returnmain();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::returnmain(){
    qDebug() << "returning";
    name="null";
    rfid="null";
    employee_id="null";
    card_id="null";
    id="null";
    i3=false;
    imiev=false;
    vitz=false;
    cargo_door=false;
    c1f1=false;
    sys=false;
    vacum=false;
    login = false;
    vitz9 = false;

    updateMain();

    ui->kcs_getKey->setVisible(false);
    ui->kcs_get_give->setVisible(false);
    ui->kcs_giveKey->setVisible(false);
    ui->kcs_mainMenu->setVisible(true);
    ui->kcs_message->setVisible(false);
    qDebug() << "finish return";


    digitalWrite(21,HIGH);
    digitalWrite(22,HIGH);
    digitalWrite(23,HIGH);
    digitalWrite(24,HIGH);
    digitalWrite(25,HIGH);
    digitalWrite(0,HIGH);
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    digitalWrite(7,HIGH);

    cardReader.run();

}

void keep_log(QString operation,QString key){
    QString newkey;
    qDebug()<<operation;
    qDebug()<< key;
    if(key =="1"){
        newkey = "i3";
    }
    else if(key =="2"){
        newkey = "iMiev";
    }
    else if(key =="3"){
        newkey = "vitz";
    }
    else if(key =="4"){
        newkey = "Meeting Room";
    }
    else if(key =="5"){
        newkey = "Cargo Door";
    }
    else if(key =="6"){
        newkey = "System Room";
    }
    else if(key =="7"){
        newkey = "Vacuum Room";
    }
    else if(key =="8"){
        newkey = "Conteiners";
    }
    else if (key =="9") {
        newkey = "vitz9";
    }

    QDate date = QDate::currentDate();
    QLocale locale = QLocale(QLocale::Turkish);
    QString new_date = locale.toString(date,"d/M/yyyy");
    QTime time  = QTime::currentTime();
    QString hour = time.toString("hh:mm:ss");
    QString query_text = QString("INSERT INTO gunseldb.kcs_log(employee_id,name_key,date,hour,operation) VALUES(%1,'%2','%3','%4','%5')").arg(employee_id).arg(newkey).arg(new_date).arg(hour).arg(operation);
    qry.exec(query_text);
}



void MainWindow::get_key(QString key){
    QString operation = "Get Key";
    QString query_text = QString("UPDATE gunseldb.keys SET status = 0, employee_id = %1 WHERE id= %2").arg(employee_id).arg(key);
    qry.exec(query_text);
    int keyid = key.toInt();
    keep_log(operation,key);
    message(operation,keyid);
}

void MainWindow::give_key(QString key){
    QString operation = "Give Key";
    QString query_text = QString("UPDATE gunseldb.keys SET status = 1, employee_id = 'Available' WHERE id= %2").arg(key);
    qry.exec(query_text);
    int keyid = key.toInt();
    keep_log(operation,key);
    message(operation,keyid);
}


void MainWindow::message(QString text, int lockNo){

    qDebug() << QString::number(lockNo);

    if(text == "Get Key"){
        ui->message->setText("Unlocking the box. \n Please take the key.");
    }
    else if (text == "Give Key"){
        ui->message->setText("Unlocking the box. \n Please return the key.");
    }
    else if (text == "notpermissions"){
        ui->message->setText("You are not authorized \n Please contact your manager.");
    }

    ui->message->setStyleSheet("font-weight: bold; font-size: 50px; color:black;");
    ui->message->setAlignment(Qt::AlignCenter);

    ui->kcs_getKey->setVisible(false);
    ui->kcs_get_give->setVisible(false);
    ui->kcs_giveKey->setVisible(false);
    ui->kcs_mainMenu->setVisible(false);
    ui->kcs_message->setVisible(true);

    switch(lockNo){
    case 1 :
        digitalWrite(21,LOW);
        break;
    case 2 :
        digitalWrite(22,LOW);
        break;
    case 3 :
        digitalWrite(23,LOW);
        break;
    case 4 :
        digitalWrite(24,LOW);
        break;
    case 5 :
        digitalWrite(25,LOW);
        break;
    case 6 :
        digitalWrite(0,LOW);
        break;
    case 7 :
        digitalWrite(2,LOW);
        break;
    case 8 :
        digitalWrite(3,LOW);
        break;
    case 9 :
        digitalWrite(7,LOW);
        qDebug() << "lastt";
        break;

    }

    pinDelay->start(5000);
    qDebug()<<"call timer";


}


void MainWindow::update_time(){
    QTime time  = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss");
    ui->time->setText(time_text);
}


//finish reading card and get id
void MainWindow::processNewCardFromDb()
{
    //get id to database
    qry.exec("SELECT id FROM user WHERE name = 'user'");
    qry.next();
    id = qry.value(0).toString();
    //clear id table
    qry.prepare("UPDATE user SET id = '0' WHERE name = 'user'");
    qry.exec();
    //if have id start next function
    if(id!="0"){
        startforenter(id);
    }
    else{
        cardReader.run();
    }
}

// if card ok , start process
void MainWindow::startforenter(QString id){
    //search id on table
    //vitz9
    QString query_text = "SELECT name,rfid,registry,i3,imiev,vitz,cargo_door,c1f2,sys,vacum,vitz9 FROM employee_iot WHERE rfid="+id;
    qry.exec(query_text);
    qry.next();
    try{
        rfid = qry.value(1).toString();
        rfid == id ? login = true : login = false;
    }
    catch(int e){

    }
    if(login == true)
    {
        name = qry.value(0).toString();
        rfid = qry.value(1).toString();
        employee_id = qry.value(2).toString();
        i3 = qry.value(3).toBool();
        imiev = qry.value(4).toBool();
        vitz = qry.value(5).toBool();
        cargo_door = qry.value(6).toBool();
        //c1f1 = qry.value(7).toBool();
        sys = qry.value(8).toBool();
        vacum = qry.value(9).toBool();
        vitz9 = qry.value(7).toBool();
        AfterLogin(employee_id);
    }
}

//check have key or not have key
void MainWindow::AfterLogin(QString employee_id){
    bool havekey = false;
    QString query_text = "SELECT employee_id FROM gunseldb.keys WHERE employee_id="+employee_id;
    qry.exec(query_text);
    while(qry.next()){
        havekey = true;
    }
    if(havekey==true){
        openGetGive();
    }
    else{
        on_get_key_clicked();
    }

}

void MainWindow::openGetGive(){
    ui->kcs_getKey->setVisible(false);
    ui->kcs_get_give->setVisible(true);
    ui->kcs_giveKey->setVisible(false);
    ui->kcs_mainMenu->setVisible(false);
    ui->kcs_message->setVisible(false);
}

//first start views
void MainWindow::start(){
    ui->kcs_getKey->setVisible(false);
    ui->kcs_get_give->setVisible(false);
    ui->kcs_giveKey->setVisible(false);
    ui->kcs_mainMenu->setVisible(true);
    ui->kcs_message->setVisible(false);
}

//for database connection
void MainWindow::connectDB(){

    db.setHostName("localhost");
    db.setUserName("dg");
    db.setPassword("password");
    db.setDatabaseName("gunseldb");
    db.open();

    if(!db.open()){
        qDebug() << "erorror";
    }
    else{
        qDebug() << "ok";
    }
}

void MainWindow::updateMain(){
    int i = 0;
    QString color[10];
    QString text[10];
    QString query_text = "SELECT * FROM gunseldb.keys";
    qry.exec(query_text);

    while(qry.next())
    {
        QString keys_employee_id = qry.value(4).toString();
        if(keys_employee_id == "Available")
        {
            keys[i]->setText(keyTexts[i] + "\nAvailable");
            keys[i]->setStyleSheet("font-weight: bold; font-size: 20px; color: green");
            keys[i]->setAlignment(Qt::AlignCenter);
        }
        else
        {
            QString query_text = "SELECT name FROM employee_iot WHERE registry="+keys_employee_id;
            QSqlQuery qry_update;
            qry_update.exec(query_text);
            qry_update.next();
            QString textToShow = qry_update.value(0).toString();

            keys[i]->setText(textToShow);
            keys[i]->setStyleSheet("font-weight: bold; font-size: 20px; color: red");
        }

        keys[i]->setAlignment(Qt::AlignCenter);
        i++;
    }
}

void MainWindow::updateGet(){
    int i = 0;
    bool status[10];
    QString query_text_2 = "SELECT * FROM gunseldb.keys";
    qry.exec(query_text_2);
    while(qry.next()){
        bool per = qry.value(3).toBool();
        per == true ? status[i] = true  : status[i] = false;
        i++;
    }

    i3 == true && status[0] == true ? ui->get_key1->setEnabled(true)  : ui->get_key1->setEnabled(false);
    imiev == true && status[1] == true ? ui->get_key2->setEnabled(true)  : ui->get_key2->setEnabled(false);
    vitz == true && status[2] == true ? ui->get_key3->setEnabled(true)  : ui->get_key3->setEnabled(false);
    c1f1 == true && status[3] == true ? ui->get_key4->setEnabled(true)  : ui->get_key4->setEnabled(false);
    cargo_door == true && status[4] == true ? ui->get_key5->setEnabled(true)  : ui->get_key5->setEnabled(false);
    sys == true && status[5] == true ? ui->get_key6->setEnabled(true)  : ui->get_key6->setEnabled(false);
    vacum == true && status[6] == true ? ui->get_key7->setEnabled(true)  : ui->get_key7->setEnabled(false);
    vitz9 == true && status[7] == true ? ui->get_key9->setEnabled(true) : ui->get_key9-> setEnabled(false);


}

void MainWindow::updateGive(){
    int i = 0;
    bool status[10];
    QString query_text = "SELECT * FROM gunseldb.keys";
    qry.exec(query_text);
    while(qry.next()){
        bool per = qry.value(3).toBool();
        if(per == false && employee_id == qry.value(4).toString() )
        {
            status[i] = true;
        }
        else
        {
            status[i] = false;
        }
        //per == false ? status[i] = true  : status[i] = false;
        i++;
    }

    ui->give_key1_->setEnabled(status[0]);
    ui->give_key2->setEnabled(status[1]);
    ui->give_key3->setEnabled(status[2]);
    ui->give_key4->setEnabled(status[3]);
    ui->give_key5->setEnabled(status[4]);
    ui->give_key6->setEnabled(status[5]);
    ui->give_key7->setEnabled(status[6]);
    //vitz9
    ui->give_key9->setEnabled(status[7]);
}



// startup widget settings
void MainWindow::startupViews(){

    QPixmap bkgnd("/home/pi/kcs/res/background2.jpg");
    bkgnd = bkgnd.scaled(this->size(),Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background,bkgnd);
    this->setPalette(palette);


    QPixmap key1_ico("/home/pi/kcs/res/i3.png");
    QPixmap key2_ico("/home/pi/kcs/res/imiev.png");
    QPixmap key3_ico("/home/pi/kcs/res/vitz.png");
    QPixmap key9_ico("/home/pi/kcs/res/vitz9.png");
    QPixmap key_ico("/home/pi/kcs/res/key.png");
    QPixmap con("/home/pi/kcs/res/con.ico");
    QPixmap get_key("/home/pi/kcs/res/out.png");
    QPixmap give_key("/home/pi/kcs/res/in.png");
    //QPixmap ico("/home/pi/kcs/res/gunsel_yatay.png");
    QPixmap cancel("/home/pi/kcs/res/cancel.png");
    QMovie *key_gif = new QMovie("/home/pi/kcs/res/key.gif");
    QMovie *unlock = new QMovie("//home/pi/kcs/res/unlock1.gif");

    ui->main_key1->setPixmap(key1_ico.scaled(200,150,Qt::KeepAspectRatio));
    ui->main_key2->setPixmap(key2_ico.scaled(200,150,Qt::KeepAspectRatio));
    ui->main_key3->setPixmap(key3_ico.scaled(200,150,Qt::KeepAspectRatio));
    //vitz9 key
    ui->main_key9->setPixmap(key9_ico.scaled(200,150,Qt::KeepAspectRatio));
    /*ui->main_key4_2->setPixmap(key_ico.scaled(45,45,Qt::KeepAspectRatio));
    ui->main_key5_2->setPixmap(key_ico.scaled(45,45,Qt::KeepAspectRatio));
    ui->main_key6_2->setPixmap(key_ico.scaled(45,45,Qt::KeepAspectRatio));
    ui->main_key_7_2->setPixmap(key_ico.scaled(45,45,Qt::KeepAspectRatio));*/
    ui->main_key4_2->setMovie(key_gif);
    ui->main_key5_2->setMovie(key_gif);
    ui->main_key6_2->setMovie(key_gif);
    ui->main_key_7_2->setMovie(key_gif);
    ui->unlock_gif->setMovie(unlock);
    unlock->start();
    key_gif->start();
    //ui->main_ico->setPixmap(ico.scaled(1024,80,Qt::KeepAspectRatio));
    //ui->main_ico->setAlignment(Qt::AlignCenter);
    //ui->message_ico->setPixmap(ico.scaled(1500,160,Qt::KeepAspectRatio));
    //ui->message_ico->setAlignment(Qt::AlignCenter);

    QFont font = ui->get_key4->font();
    font.setBold(15);
    font.setPointSize(16);


    ui->get_key1->setIcon(key1_ico.scaled(250,200,Qt::KeepAspectRatio));
    ui->get_key1->setIconSize(key1_ico.rect().size());
    ui->get_key2->setIcon(key2_ico.scaled(250,200,Qt::KeepAspectRatio));
    ui->get_key2->setIconSize(key2_ico.rect().size());
    ui->get_key3->setIcon(key3_ico.scaled(250,200,Qt::KeepAspectRatio));
    ui->get_key3->setIconSize(key2_ico.rect().size());
    //vitz9
    ui->get_key9->setIcon(key9_ico.scaled(250,200,Qt::KeepAspectRatio));
    ui->get_key9->setIconSize(key2_ico.rect().size());



    ui->get_key4->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->get_key4->setIcon(key_ico);
    ui->get_key4->setIconSize(QSize(100,100));
    ui->get_key4->setText("Meeting Room");
    ui->get_key4->setFont(font);

    ui->get_key5->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->get_key5->setIcon(key_ico);
    ui->get_key5->setIconSize(QSize(100,100));
    ui->get_key5->setText("Cargo Door");
    ui->get_key5->setFont(font);

    ui->get_key6->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->get_key6->setIcon(key_ico);
    ui->get_key6->setIconSize(QSize(100,100));
    ui->get_key6->setText("System Room");
    ui->get_key6->setFont(font);

    ui->get_key7->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->get_key7->setIcon(key_ico);
    ui->get_key7->setIconSize(QSize(100,100));
    ui->get_key7->setText("Vacuum Room");
    ui->get_key7->setFont(font);

    ui->get_key8->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->get_key8->setIcon(con);
    ui->get_key8->setIconSize(QSize(100,100));
    ui->get_key8->setText("Containers");
    ui->get_key8->setFont(font);

    ui->give_key1_->setIcon(key1_ico.scaled(250,200,Qt::KeepAspectRatio));
    ui->give_key1_->setIconSize(key1_ico.rect().size());

    ui->give_key2->setIcon(key2_ico.scaled(250,200,Qt::KeepAspectRatio));
    ui->give_key2->setIconSize(key2_ico.rect().size());

    ui->give_key3->setIcon(key3_ico.scaled(250,200,Qt::KeepAspectRatio));
    ui->give_key3->setIconSize(key2_ico.rect().size());

   //vitz9
    ui->give_key9->setIcon(key9_ico.scaled(250,200,Qt::KeepAspectRatio));
    ui->give_key9->setIconSize(key2_ico.rect().size());

    ui->give_key4->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->give_key4->setIcon(key_ico);
    ui->give_key4->setIconSize(QSize(100,100));
    ui->give_key4->setText("Meeting Room");
    ui->give_key4->setFont(font);

    ui->give_key5->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->give_key5->setIcon(key_ico);
    ui->give_key5->setIconSize(QSize(100,100));
    ui->give_key5->setText("Cargo Door");
    ui->give_key5->setFont(font);

    ui->give_key6->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->give_key6->setIcon(key_ico);
    ui->give_key6->setIconSize(QSize(100,100));
    ui->give_key6->setText("System Room");
    ui->give_key6->setFont(font);

    ui->give_key7->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->give_key7->setIcon(key_ico);
    ui->give_key7->setIconSize(QSize(100,100));
    ui->give_key7->setText("Vacuum Room");
    ui->give_key7->setFont(font);

    ui->give_key8->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->give_key8->setIcon(con);
    ui->give_key8->setIconSize(QSize(100,100));
    ui->give_key8->setText("Containers");
    ui->give_key8->setFont(font);

    ui->get_key->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->get_key->setIcon(get_key);
    ui->get_key->setIconSize(QSize(150,150));
    ui->get_key->setText("Get Key");
    ui->get_key->setFont(font);

    ui->give_key->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->give_key->setIcon(give_key);
    ui->give_key->setIconSize(QSize(150,150));
    ui->give_key->setText("Give Key");
    ui->give_key->setFont(font);

    ui->get_cancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->get_cancel->setIcon(cancel);
    ui->get_cancel->setIconSize(QSize(75,75));
    ui->get_cancel->setText("Cancel");
    ui->get_cancel->setFont(font);

    ui->give_cancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->give_cancel->setIcon(cancel);
    ui->give_cancel->setIconSize(QSize(75,75));
    ui->give_cancel->setText("Cancel");
    ui->give_cancel->setFont(font);

    ui->kcs_getKey->setVisible(false);
    ui->kcs_get_give->setVisible(false);
    ui->kcs_giveKey->setVisible(false);
    ui->kcs_mainMenu->setVisible(false);
    ui->kcs_message->setVisible(false);
}

void MainWindow::on_get_key_clicked()
{
    updateGet();
    ui->kcs_getKey->setVisible(true);
    ui->kcs_get_give->setVisible(false);
    ui->kcs_giveKey->setVisible(false);
    ui->kcs_mainMenu->setVisible(false);
    ui->kcs_message->setVisible(false);
}


void MainWindow::on_give_key_clicked()
{
    updateGive();
    ui->kcs_getKey->setVisible(false);
    ui->kcs_get_give->setVisible(false);
    ui->kcs_giveKey->setVisible(true);
    ui->kcs_mainMenu->setVisible(false);
    ui->kcs_message->setVisible(false);
}




void MainWindow::on_give_key1__clicked()
{
    give_key("1");
}


void MainWindow::on_give_key2_clicked()
{
    give_key("2");
}

void MainWindow::on_give_key3_clicked()
{
    give_key("3");
}

void MainWindow::on_give_key4_clicked()
{
    give_key("4");
}

void MainWindow::on_give_key5_clicked()
{
    give_key("5");
}

void MainWindow::on_give_key6_clicked()
{
    give_key("6");
}

void MainWindow::on_give_key7_clicked()
{
    give_key("7");
}

void MainWindow::on_give_key8_clicked()
{
    give_key("8");
}

//vitz9
void MainWindow::on_give_key9_clicked()
{
    give_key("9");
}

void MainWindow::on_get_key1_clicked()
{
    qDebug()<<"geti3333";
    get_key("1");
}

void MainWindow::on_get_key2_clicked()
{
    get_key("2");
}

void MainWindow::on_get_key3_clicked()
{
    get_key("3");
}

void MainWindow::on_get_key4_clicked()
{
    get_key("4");
}

void MainWindow::on_get_key5_clicked()
{
    get_key("5");
}

void MainWindow::on_get_key6_clicked()
{
    get_key("6");
}

void MainWindow::on_get_key7_clicked()
{
    get_key("7");
}

void MainWindow::on_get_key8_clicked()
{
    get_key("8");
}

//vitz9
void MainWindow::on_get_key9_clicked()
{
    get_key("9");
}

void MainWindow::on_give_cancel_clicked()
{
    returnmain();
}

void MainWindow::on_get_cancel_clicked()
{
    returnmain();
}

void MainWindow::on_pushButton_clicked()
{
    QProcess::execute("reboot");
}
