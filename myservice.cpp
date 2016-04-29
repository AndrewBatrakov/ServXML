#include "myservice.h"
#include "readxml.h"

MyService::MyService(int argc, char **argv) : QtService<QCoreApplication>(argc,argv,"XML to SQL")
{
    try {
        setServiceDescription("Service XML to SQL");
        setServiceFlags(QtServiceBase::CannotBeStopped);
    } catch (...) {
        qCritical() << "An unknown error in constructor";
    }
}

MyService::~MyService()
{
    try {

    } catch (...) {
        qCritical() << "An unknown error in destructor";
    }
}

void MyService::start()
{
    try {
        QCoreApplication *app = application();
        qDebug() << "Service started!1";
        qDebug() << app->applicationDirPath();

        //My Class
        readXML = new ReadXML;
    } catch (...) {
        qCritical() << "An unknown start error";
    }
}

void MyService::pause()
{
    try {
        qDebug() << "Service paused!";
    } catch (...) {
        qCritical() << "An unknown pause error";
    }
}

void MyService::resume()
{
    try {
        qDebug() << "Service resume!";
    } catch (...) {
        qCritical() << "An unknown resume error";
    }
}

void MyService::stop()
{
    try {
        qDebug() << "Service stopped!";
    } catch (...) {
        qCritical() << "An unknown stop error";
    }
}

