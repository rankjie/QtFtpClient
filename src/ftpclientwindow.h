#ifndef FTPCLIENTWINDOW_H
#define FTPCLIENTWINDOW_H

//#include <QtGui/QDialog>
#include <QtGui>
#include "ui_ftpclientwindow.h"
#include "ftpclient.h"


class FtpClientWindow : public QDialog,    Ui::FtpClientWindowClass
{
    Q_OBJECT

public:
    FtpClientWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~FtpClientWindow();

private slots:
	void changeServerName();
	void changeServerPort();
	void changeUserName();
	void changePWD();

	void ftpConnect();
	void processItem(QListWidgetItem *item);
	void switchDownloadButton();
	void cdParent();
	void downloadFile();
        void putloadFile();
        void chooseFile();
	void changeList(const QUrlInfo &urlInfo);
	void onConnect(bool);
	void onGot(bool);
        void onPut(bool);
	void onList(bool);
        //void onTopDir(bool);
        void on_delbutton_clicked();

private:
	FtpClient* ftpClient;
};

#endif // FTPCLIENTWINDOW_H
