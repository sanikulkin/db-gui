#include "opendialog.h"
#include "ui_opendialog.h"

OpenDialog::OpenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenDialog) {
    ui->setupUi(this);
}

OpenDialog::~OpenDialog() {
    delete ui;
}

QString OpenDialog::GetDbName() {
    return ui->dbNameEdit->text();
}

QString OpenDialog::GetHost() {
    return ui->hostEdit->text();
}

int OpenDialog::GetPort() {
    return ui->portEdit->text().toInt();
}

QString OpenDialog::GetLogin() {
    return ui->loginEdit->text();
}

QString OpenDialog::GetPassword() {
    return ui->passwordEdit->text();
}
