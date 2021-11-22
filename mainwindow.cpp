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

    settings = new QSettings("../db-gui/malik_loh.ini", QSettings::IniFormat, this);
}

MainWindow::~MainWindow() {
    delete ui;
    delete settings;
}

void MainWindow::on_action_triggered() {
    OpenDialog openDia;
    ConnectInfo info;
    bool flag = true;
    if (flag)
        LoadInfo(info);
    if (!flag && openDia.exec() == QDialog::Accepted) {
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

void MainWindow::setDb(const QSqlDatabase& p_db) {
    db = p_db;
}

QSqlDatabase MainWindow::ConnectToDb(ConnectInfo &info) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "myDb");
    if (info.host == "hui") {
        info.host = "195.19.32.74";
        info.port = 5432;
        info.dbName = "fn1132_2021";
        info.userName = "student";
        info.password = "bmstu";
    }
    db.setHostName(info.host);
    db.setPort(info.port);
    db.setDatabaseName(info.dbName);
    db.setUserName(info.userName);
    db.setPassword(info.password);

    if (!db.open()) {
       PrintErrors("Database connection faild");
    }
    else {
        PrintErrors("Well done!");
    }

    SaveInfo(info);
    return db;
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
        PrintErrors("Well done");
        PrintHistory(queryStr);
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
    QString res;
    bool flag = true;
    for (int i = 0; i < query.size(); ++i) {
        if (query[i] == '\t' || query[i] == '\n' || query[i] == ' ') {
            if (flag) {
                res.push_back(' ');
                flag = false;
            }
        }
        else {
            res.push_back(query[i]);
            flag = true;
        }
    }
    ui->historyEdit->appendPlainText(res);
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
