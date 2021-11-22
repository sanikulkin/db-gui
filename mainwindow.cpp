#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opendialog.h"
#include "disconnect.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,0);

    connect(ui->requestButton,SIGNAL(clicked()),this,SLOT(SendButton()));
    connect(ui->clearButton,SIGNAL(clicked()),this,SLOT(ClearButton()));
    connect(ui->historyButton,SIGNAL(clicked()),this,SLOT(ClearHistoryButton()));

    settings = new QSettings("../db-gui/config.ini", QSettings::IniFormat, this);
    LoadHistory();
}

MainWindow::~MainWindow() {
    db.close();
    delete ui;
    delete settings;
}

void MainWindow::on_action_triggered() {
    OpenDialog openDia;
    ConnectInfo info;
    LoadInfo(info);
    openDia.SetDbName(info.dbName);
    openDia.SetHost(info.host);
    openDia.SetPort(QString(std::to_string(info.port).c_str()));
    openDia.SetLogin(info.userName);
    openDia.SetPassword(info.password);
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
}

void MainWindow::on_actionDisconnect_triggered() {
    Disconnect dcon;
    if (dcon.exec() == QDialog::Accepted) {
        DisconnectFromDb();
    }
    ClearTables();
}

void MainWindow::setDb(const QSqlDatabase& p_db) {
    db = p_db;
}

QSqlDatabase MainWindow::ConnectToDb(ConnectInfo &info) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "myDb");
    db.setHostName(info.host);
    db.setPort(info.port);
    db.setDatabaseName(info.dbName);
    db.setUserName(info.userName);
    db.setPassword(info.password);
    if (!db.open()) {
       PrintErrors("Database connection failed");
    } else {
        PrintErrors("Connected to database!");
    }
    SaveInfo(info);
    return db;
}

void MainWindow::DisconnectFromDb() {
    db.removeDatabase("myDb");
}

QString MainWindow::GetQuery() {
    return ui->comandEdit->toPlainText();
}

void MainWindow::MakeQuery(const QString& queryStr) {
    QSqlQuery* query = new QSqlQuery(db);
    QSqlQueryModel *setquery = new QSqlQueryModel;
    if (query->exec(queryStr)) {
        setquery->setQuery(*query);
        ui->resultView->setModel(setquery);
        PrintErrors("Well done!");
        PrintHistory(queryStr);
    } else if (queryStr.length() == 0) {
        PrintErrors("Empty query");
    } else {
       PrintErrors(query->lastError().text());
    }
}

void MainWindow::SendButton() {
    if (!db.isValid()) {
        PrintErrors("No connection to database");
        return;
    }
    QString query = GetQuery();
    MakeQuery(query);
}

void MainWindow::ClearButton() {
    ui->comandEdit->clear();
    ui->errorLable->clear();
}

void MainWindow::ClearHistoryButton() {
    ui->historyEdit->clear();
}

void MainWindow::PrintTables() {
    QStringListModel* tables = new QStringListModel(ui->tableList);
    tables->setStringList(db.tables());
    ui->tableList->setModel(tables);
}

void MainWindow::PrintHistory(const QString& query) {
    QString res;
    bool flag = true;
    for (int i = 0; i < query.size(); ++i) {
        if (query[i] == '\t' || query[i] == '\n' || query[i] == ' ') {
            if (flag) {
                res.push_back(' ');
                flag = false;
            }
        } else {
            res.push_back(query[i]);
            flag = true;
        }
    }
    ui->historyEdit->appendPlainText(res);
    SaveHistory();
}

void MainWindow::PrintErrors(const QString& error) {
   ui->errorLable->setText(error);
}

void MainWindow::SaveInfo(const ConnectInfo &info) {
    settings->setValue("dbName", info.dbName);
    settings->setValue("host", info.host);
    settings->setValue("password", info.password);
    settings->setValue("port", info.port);
    settings->setValue("userName", info.userName);
}

void MainWindow::LoadInfo(ConnectInfo& info) {
    info.dbName = settings->value("dbName").toString();
    info.host = settings->value("host").toString();
    info.password = settings->value("password").toString();
    info.port = settings->value("port").toInt();
    info.userName = settings->value("userName").toString();
}

void MainWindow::SaveHistory() {
    QString str = ui->historyEdit->toPlainText();
    settings->setValue("history", str);
}

void MainWindow::LoadHistory() {
    QString str = settings->value("history").toString();
    ui->historyEdit->appendPlainText(str);
}

void MainWindow::ClearTables() {
    QStringListModel* tables = new QStringListModel{};
    ui->tableList->setModel(tables);
}
