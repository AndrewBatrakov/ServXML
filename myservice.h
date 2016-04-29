#ifndef MYSERVICE_H
#define MYSERVICE_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include "qtservice.h"
#include "readxml.h"

class MyService : public QtService<QCoreApplication>
{
public:
    MyService(int argc, char **argv);
    ~MyService();

    void start();
    void pause();
    void resume();
    void stop();

private:
     ReadXML *readXML;
};

#endif // MYSERVICE_H
