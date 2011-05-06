
#include "ftpclientwindow.h"
#include <QtNetwork>

FtpClientWindow::FtpClientWindow(QWidget *parent, Qt::WFlags flags)
    : QDialog(parent, flags)
{
		setupUi(this);
		ftpClient = new FtpClient(this);

		connect(ftpServerLineEdit, SIGNAL(editingFinished ()),
			this, SLOT(changeServerName()));
                //connect(ftpServerPortLineEdit, SIGNAL(editingFinished ()),
                //	this, SLOT(changeServerPort()));
                //connect(userNamelineEdit, SIGNAL(editingFinished ()),
                //	this, SLOT(changeUserName()));
                //connect(passWordlineEdit, SIGNAL(editingFinished ()),
                //	this, SLOT(changePWD()));


		connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
			this, SLOT(processItem(QListWidgetItem *)));
		connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
			this, SLOT(switchDownloadButton()));
		connect(connectPB, SIGNAL(clicked()), this, SLOT(ftpConnect()));
                //connect(cdUpDirPB, SIGNAL(clicked()), this, SLOT(cdParent()));
		connect(downLoadPB, SIGNAL(clicked()), this, SLOT(downloadFile()));
                connect(putLoadPB, SIGNAL(clicked()), this, SLOT(putloadFile()));

                connect(choosePB, SIGNAL(clicked()), this, SLOT(chooseFile()));
		connect(ftpClient, SIGNAL(cmdChangeList(const QUrlInfo &)), 
			this, SLOT(changeList(const QUrlInfo &)));

		connect(ftpClient, SIGNAL(cmdConncted(bool)), 
			this, SLOT(onConnect(bool)));
		connect(ftpClient, SIGNAL(cmdGot(bool)), 
			this, SLOT(onGot(bool)));
                connect(ftpClient, SIGNAL(cmdPut(bool)),
                        this, SLOT(onPut(bool)));
		connect(ftpClient, SIGNAL(cmdList(bool)), 
			this, SLOT(onList(bool)));		
                //connect(ftpClient, SIGNAL(cmdIsTopDir(bool)),
                //	this, SLOT(onTopDir(bool)));

		connect(quitPB, SIGNAL(clicked()), this, SLOT(close()));

}

FtpClientWindow::~FtpClientWindow()
{

}
void FtpClientWindow::changeServerName()
{
        ftpClient->setserverName(ftpServerLineEdit->text());
}

void FtpClientWindow::changeServerPort()
{
        ftpClient->setserverPort("21");   // I don't think this should be changable.
}

void FtpClientWindow::changeUserName()
{
        ftpClient->setuserName(username->text());
}

void FtpClientWindow::changePWD()
{
        ftpClient->setpassWord(password->text());
}

void FtpClientWindow::ftpConnect()
{
	changeServerName();
	changeServerPort();
	changeUserName();
	changePWD();
	ftpClient-> ftpConnect();
}

void FtpClientWindow::processItem(QListWidgetItem *item)
{
	ftpClient->changeDir( item->text());
	listWidget->clear();
}

void FtpClientWindow::cdParent()
{
	ftpClient->changeDir("");
	listWidget->clear();
}

void FtpClientWindow::switchDownloadButton()
{
	QListWidgetItem *current = listWidget->currentItem();
	if (current) {
		downLoadPB->setEnabled(!ftpClient->isDir(current->text()));
	} else {
		downLoadPB->setEnabled(false);
	}
}

void FtpClientWindow::downloadFile()
{
	QString fileName = listWidget->currentItem()->text();
	ftpClient->getFile(fileName);
}
void FtpClientWindow::putloadFile()
{
        //QString fileName = listWidget->currentItem()->text();
        //ftpClient->getFile(fileName);
        QString currentPath=displayTextEdit->text();
        QString fileName= currentPath.right(currentPath.length()-currentPath.lastIndexOf('/'));
       if( !currentPath.isEmpty())
       {
               QFile *remoteFileName=new QFile(currentPath);
               ftpClient->putFile(remoteFileName, fileName);
       }
}

void FtpClientWindow::changeList(const QUrlInfo &urlInfo)
{
	QListWidgetItem *item = new QListWidgetItem;
	item->setText(urlInfo.name());
	QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
	item->setIcon(pixmap);
	listWidget->addItem(item);
	if (!listWidget->currentItem()) {
		listWidget->setCurrentItem(listWidget->item(0));
		listWidget->setEnabled(true);
	}
}

void FtpClientWindow::onConnect(bool isconnect)
{
	if(isconnect){
		listWidget->clear();
		listWidget->setEnabled(true);
		connectPB->setText(tr("断开"));
	}
	else{
		listWidget->setEnabled(false);
		connectPB->setText(tr("连接"));
	}
	statusLab->setText(ftpClient->getStatus());
}

void FtpClientWindow::onGot(bool /*error*/)
{
        switchDownloadButton();
	statusLab->setText(ftpClient->getStatus());
}
void FtpClientWindow::onPut(bool /*error*/)
{
        //switchDownloadButton();
        statusLab->setText(tr("上传文件 %1 到指定目录"));
}

void FtpClientWindow::onList(bool error)
{
	if(error){
		listWidget->addItem(tr("<empty>"));
		listWidget->setEnabled(false);
	}
	statusLab->setText(ftpClient->getStatus());
}
void FtpClientWindow::chooseFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "/usr/local/Trolltech",
                               tr("任何文件(*.*)" ";;文本文件(*.txt)"";;XML文件(*.xml)"));
    displayTextEdit->setText(fileName);

}
/*void FtpClientWindow::onTopDir(bool istop)
{

	if(istop){
		cdUpDirPB->setEnabled(false);
	}
	else{
		cdUpDirPB->setEnabled(true);
	}
	statusLab->setText(ftpClient->getStatus());
}*/
