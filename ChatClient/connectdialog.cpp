#include "connectdialog.h"
#include "ui_connectdialog.h"

#include "client.h"

#include <QHostAddress>


ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

// Tries connecting to the specified address
void ConnectDialog::on_connectButton_clicked()
{
    Client::Join(QHostAddress(ui->addressBar->text()));
	this->close();
}

void ConnectDialog::on_addressBar_returnPressed()
{
    // Does the same thing as pressing the connect button
    on_connectButton_clicked();
}
