#include <QThread>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"

#include "client.h"
#include "common.h"

#include <QKeyEvent>

const unsigned short COMMANDBUFFER_LIMIT = 32;
int currentCommandIndex = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	ui->lineEdit->installEventFilter(this);

	InitClient();
}

MainWindow::~MainWindow()
{
    delete ui;

	Client::Send(CMD_DISCONNECTFROMSERVER.Key());
}

void MainWindow::GoUpInCommandBuffer()
{
	if (currentCommandIndex < commandBuffer.size())
	{
		ui->lineEdit->setText(*commandBuffer[currentCommandIndex++]);
	}
}

void MainWindow::InitClient()
{
	Client::StartThread(this);
}

// Prints the received message and snaps the scrollbar if it's on the bottom
void MainWindow::PrintMessage(QString message)
{
    ui->textBrowser->append(message);
}

// Clears all the messages in the text browser
void MainWindow::ClearMessages()
{
	ui->textBrowser->setText("");
}

// Sends a message from the chat window
void MainWindow::on_lineEdit_returnPressed()
{
	// Sends the message via the QUdpSocket in Client
    Client::Send(ui->lineEdit->text().toStdString());

	// Checks if we used the 'up' key to use an old message
	if (currentCommandIndex == 0)
	{
		// If so, adds that message to 
		commandBuffer.push_front(new QString(ui->lineEdit->text()));

		if (commandBuffer.size() > COMMANDBUFFER_LIMIT)
		{
			delete commandBuffer.back();
			commandBuffer.pop_back();
		}
	}
	else if(ui->lineEdit->text().size() != 0)
	{
		for (int i = 0; i < currentCommandIndex - 1; i++)
		{
			delete commandBuffer.front();
			commandBuffer.pop_front();
		}
	}

	currentCommandIndex = 0;
    ui->lineEdit->clear();
}


void MainWindow::on_actionConnect_triggered()
{
    ConnectDialog dialog;
    dialog.setModal(true);
    dialog.exec();
}


void MainWindow::on_actionDisconnect_triggered()
{
	Client::Send(CMD_DISCONNECTFROMSERVER.Key());
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == ui->lineEdit && event->type() == QKeyEvent::KeyPress)
	{
		QKeyEvent* key = static_cast<QKeyEvent*>(event);
		if(key->key() == Qt::Key_Up)
		{ 
			GoUpInCommandBuffer();
		}
	}
	return QObject::eventFilter(obj, event);
}