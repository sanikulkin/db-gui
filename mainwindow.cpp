#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opendialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

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
