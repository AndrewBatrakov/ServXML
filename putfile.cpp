#include "putfile.h"

PutFile::PutFile()
{
}

PutFile::~PutFile()
{
    filePut->close();
    filePut->remove();
}

void PutFile::putFile(QString nameOfFile)
{
    filePut = new QFile;
    filePut->setFileName(nameOfFile);
    filePut->open(QIODevice::ReadOnly);

    QString name = "ftp://ftp.our-employ.nichost.ru/";
    //name += "/QtProject/ServerEmployee/Change/";
    name += nameOfFile;

    qDebug()<<name;
    QUrl url(name);
    url.setUserName("our-employ_ftp");
    url.setPassword("W_8QCQW");
    url.setPort(21);

    QEventLoop loop;
    manager = new QNetworkAccessManager(0);
    reply = manager->put(QNetworkRequest(url),filePut);
    connect(reply,SIGNAL(finished()),&loop,SLOT(quit()));
    qDebug()<<reply;
    loop.exec();

}
