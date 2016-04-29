#include "readxml.h"

ReadXML::ReadXML()
{
    QDir::setCurrent("d:/apache/QtProject/ServerEmployee/Change/");
    watcher = new QFileSystemWatcher;
    watcher->addPath("d:/apache/QtProject/ServerEmployee/Change/");
    connect(watcher,SIGNAL(directoryChanged(QString)),SLOT(updateDir(QString)));
    connect(watcher,SIGNAL(fileChanged(QString)),SLOT(updateDir(QString)));
}

void ReadXML::updateDir(QString dir)
{
qDebug()<<"Hello! "<<dir;
}

void ReadXML::updateBase()
{

}

void ReadXML::traverseAllInformation(const QDomNode &)
{

}
