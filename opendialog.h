#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QDialog>

namespace Ui {
class OpenDialog;
}

class OpenDialog : public QDialog {
    Q_OBJECT

public:
    explicit OpenDialog(QWidget *parent = nullptr);
    ~OpenDialog();
    QString GetDbName();
    QString GetHost();
    int GetPort();
    QString GetLogin();
    QString GetPassword();
    void SetDbName(const QString& dbName);
    void SetHost(const QString& host);
    void SetPort(const QString& host);
    void SetLogin(const QString& login);
    void SetPassword(const QString& pass);

private:
    Ui::OpenDialog *ui;
};

#endif // OPENDIALOG_H
