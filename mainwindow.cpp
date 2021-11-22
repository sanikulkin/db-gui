#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opendialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,0);

    connect(ui->requestButton,SIGNAL(clicked()),this,SLOT(SendButton()));
    connect(ui->clearButton,SIGNAL(clicked()),this,SLOT(ClearButton()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_action_triggered() {
    OpenDialog openDia;
    ConnectInfo info;
    if (openDia.exec() == QDialog::Accepted) {
        info.host = openDia.GetHost();
        info.port = openDia.GetPort();
        info.dbName = openDia.GetDbName();
        info.userName = openDia.GetLogin();
        info.password = openDia.GetPassword();
    }
    QSqlDatabase db = ConnectToDb(info);
    setDb(db);
    PrintTables();
    SaveInfo(info);
}

void MainWindow::setDb(const QSqlDatabase& p_db) {
    db = p_db;
}

QSqlDatabase MainWindow::ConnectToDb(const ConnectInfo &info) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "myDb");
    if (info.host == "hui") {
        db.setHostName("195.19.32.74");
        db.setPort(5432);
        db.setDatabaseName("fn1132_2021");
        db.setUserName("student");
        db.setPassword("bmstu");
    } else {
        db.setHostName(info.host);
        db.setPort(info.port);
        db.setDatabaseName(info.dbName);
        db.setUserName(info.userName);
        db.setPassword(info.password);
    }

    if (!db.open()) {
       PrintErrors("Database connection faild");
    }

    return db;
}

void MainWindow::SendButton() {
    if (!db.isValid()) {
        PrintErrors("No connection to database");
        return;
    }
    QSettings settings;
    int size = settings.beginReadArray("connectInfo");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        qDebug() << settings.value("dbName").toString();
        qDebug() << settings.value("host").toString();
        qDebug() << settings.value("password").toString();
        qDebug() << settings.value("port").toString();
        qDebug() << settings.value("userName").toString();
    }
    settings.endArray();
    QString query = GetQuery();
    MakeQuery(query);
    PrintHistory(query);
}

void MainWindow::ClearButton() {
    ui->comandEdit->clear();
    ui->errorLable->clear();
}

QString MainWindow::GetQuery() {
    return ui->comandEdit->toPlainText();
}

void MainWindow::MakeQuery(const QString& queryStr) {
    QSqlQuery* query = new QSqlQuery(db);
    QSqlQueryModel *setquery = new QSqlQueryModel;
    if (query->prepare(queryStr)) {
        query->exec();
        setquery->setQuery(*query);
        ui->resultView->setModel(setquery);
        ui->errorLable->setText("Well done");
    } else if (queryStr.length() == 0) {
        PrintErrors("EmptyQuery");
    } else {
       PrintErrors(query->lastError().text());
    }
}

void MainWindow::PrintTables() {
    QStringListModel* tables = new QStringListModel(ui->tableList);
    tables->setStringList(db.tables());
    ui->tableList->setModel(tables);
}

void MainWindow::PrintHistory(const QString& query) {
    ui->historyEdit->appendPlainText(query+"\n");
}

void MainWindow::PrintErrors(const QString& error) {
   ui->errorLable->setText(error);
}

void MainWindow::SaveInfo(const ConnectInfo &info) {
    QSettings settings("./info.txt", QSettings::defaultFormat());
    settings.beginWriteArray("connectInfo");
    settings.setValue("dbName", info.dbName);
    settings.setValue("host", info.host);
    settings.setValue("password", info.password);
    settings.setValue("port", info.port);
    settings.setValue("userName", info.userName);
    settings.endArray();
    settings.sync();
}
