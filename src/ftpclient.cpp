#include "ftpclient.h"
#include <QtNetwork>
#include <QtGui>

FtpClient::FtpClient(QObject *parent)
	: QObject(parent),ftp(NULL),file(NULL)
{
	progressDialog = new QProgressDialog((QWidget *)parent);
}

FtpClient::~FtpClient()
{

}

void FtpClient::ftpConnect()
{
	if (ftp ==NULL) {
		QApplication::setOverrideCursor(Qt::WaitCursor);
		ftp = new QFtp(this);
		connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
		connect(ftp, SIGNAL(commandFinished(int, bool)),
			this, SLOT(ftpCommandFinished(int, bool)));
		connect(ftp, SIGNAL(listInfo(const QUrlInfo &)),
			this, SLOT(addToList(const QUrlInfo &)));
		connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)),
			this, SLOT(updateDataTransferProgress(qint64, qint64)));

		ftp->connectToHost(serverName,serverPort.toUShort());
		ftp->login(userName,passWord);
		ftp->list();
		ftpStatus = tr("连接FTP服务器 %1...")
			.arg(serverName);
		emit cmdConncted(true);
	}
	else {
		ftp->abort();
		ftp->deleteLater();
		ftp = NULL;
		emit cmdConncted(false);
	}

}

void FtpClient::cancelDownload()
{
		ftp->abort();
}

void FtpClient::ftpCommandFinished(int /*commandId*/, bool error)
{

	if (ftp->currentCommand() == QFtp::Login) {
		if (error) {
			QApplication::restoreOverrideCursor();
			ftpStatus =tr("无法连接FTP服务器： "
				"%1. 请检查主机名、端口、用户名和密码是否正确 ")
				.arg(serverName);
			emit cmdConncted(false);
			return;
		}
		ftpStatus =tr("已连接FTP服务器：%1。 ").arg(serverName);
		emit cmdConncted(true);
		return;
	}
        /*if (ftp->currentCommand() == QFtp::Put) {
                QApplication::restoreOverrideCursor();

                        ftpStatus = tr("上传文件 %1 到指定目录")
                                .arg(file->fileName());

                        emit cmdPut(false);

            }*/
	if (ftp->currentCommand() == QFtp::Get) {
		QApplication::restoreOverrideCursor();
		if (error) {
			ftpStatus =tr("取消下载文件%1.")
				.arg(file->fileName());
			file->close();
			file->remove();
			emit cmdGot(true);
		} else {
			ftpStatus = tr("下载文件 %1 到当前目录")
				.arg(file->fileName());
			file->close();
			emit cmdGot(false);
		}
		delete file;
	} else if (ftp->currentCommand() == QFtp::List) {
		QApplication::restoreOverrideCursor();
		if (isDirectory.isEmpty()) {
			emit cmdList(true);
		}
	}
}

void FtpClient::addToList(const QUrlInfo &urlInfo)
{

	isDirectory[urlInfo.name()] = urlInfo.isDir();
	emit cmdChangeList(urlInfo);
}


void FtpClient::changeDir(const QString& dir)
{
	if(dir.isEmpty())//返回父目录
	{
		QApplication::setOverrideCursor(Qt::WaitCursor);
		isDirectory.clear();
		currentPath = currentPath.left(currentPath.lastIndexOf('/'));
		if (currentPath.isEmpty()) {
			ftp->cd("/");
			emit cmdIsTopDir(true);
		} else {
			ftp->cd(currentPath);
		}
		ftp->list();
	}
	else//进入子目录
	{
		if (isDirectory.value(dir)) {
			isDirectory.clear();
			currentPath += "/" + dir;
			ftp->cd(dir);
			ftp->list();
			emit cmdIsTopDir(false);
			QApplication::setOverrideCursor(Qt::WaitCursor);
		}
	}
}

void FtpClient::getFile(const QString& fileName)
{
	if (QFile::exists(fileName)) {
		QMessageBox::information(0, tr("FTP"),
			tr("%1文件已存在。")
			.arg(fileName));
		return;
	}

	file = new QFile(fileName);
	if (!file->open(QIODevice::WriteOnly)) {
		QMessageBox::information(0, tr("FTP"),
			tr("无法保存文件 %1: %2.")
			.arg(fileName).arg(file->errorString()));
		delete file;
		return;
	}

	ftp->get(fileName, file);

	progressDialog->setLabelText(tr("下载文件%1...").arg(fileName));
	progressDialog->show();
	emit cmdDownloading();

}
void FtpClient::putFile(QIODevice* remoteFileName, const QString& fileName)
{
    ftp->put(remoteFileName, fileName);
    progressDialog->setLabelText(tr("上传文件%1...").arg(fileName));
    progressDialog->show();
   // ftpStatus = tr("上传文件 %1 到指定目录").arg(file->fileName());
     emit cmdPut(true);
}
void FtpClient::updateDataTransferProgress(qint64 readBytes,qint64 totalBytes)
{
	progressDialog->setMaximum(totalBytes);
	progressDialog->setValue(readBytes);

}
