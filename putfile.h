#ifndef PUTFILE_H
#define PUTFILE_H

#include <QtNetwork>

class PutFile : public QObject
{
    Q_OBJECT
public:
    PutFile();
    ~PutFile();

public slots:
    void putFile(QString nameOfFile);

private:
    QFile *filePut;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QUrl url;
};


#endif // PUTFILE_H
