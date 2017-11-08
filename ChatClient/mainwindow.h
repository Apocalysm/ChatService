#pragma once
#include <QMainWindow>
#include <deque>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void PrintMessage(QString message);
    void ClearMessages();

	std::deque<QString*> commandBuffer;

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_lineEdit_returnPressed();
    void on_actionConnect_triggered();
    void on_actionDisconnect_triggered();

private:
	Ui::MainWindow *ui;

	void GoUpInCommandBuffer();

	void InitClient();
};
