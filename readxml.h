#ifndef READXML_H
#define READXML_H

#include <QObject>
#include <QtCore>
#include <QtXml/QDomNode>
#include <QtSql>

class ReadXML : public QObject
{
    Q_OBJECT

public:
    ReadXML();

private slots:
    void updateDir(QString dir);
    void updateBase();
    void traverseAllInformation(const QDomNode &);

private:
    QSqlDatabase dataBase;
    QFileSystemWatcher *watcher;
    QFile exchangeFile;
    bool modifyBase;
};
#endif // READXML_H
