#include <QCoreApplication>
#include <QtCore>
#include "myservice.h"
#include "readxml.h"

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);
//    return a.exec();
    MyService service(argc, argv);

//ReadXML readXML;
//readXML.updateTime();

    return service.exec();
}
