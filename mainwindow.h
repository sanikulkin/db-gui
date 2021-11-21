#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QDebug>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


struct ConnectInfo {
    QString host;
    int port;
    QString dbName;
    QString userName;
    QString password;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setDb(const QSqlDatabase& db);

private slots:
    void on_action_triggered();
    QString GetQuery();
    void SendButton();
    void ClearButton();

private:
    QSqlDatabase db;
    Ui::MainWindow *ui;
    QSqlDatabase ConnectToDb(const ConnectInfo& info);
    void MakeQuery(const QString& query);
    void PrintTables();
    void PrintHistory(const QString& query);
    void PrintErrors(const QString& error);
};

#endif // MAINWINDOW_H
