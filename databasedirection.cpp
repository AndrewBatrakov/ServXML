#include "databasedirection.h"

DataBaseDirection::DataBaseDirection()
{
}

bool DataBaseDirection::connectDataBase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("d:/apache/QtProject/ServerEmployee/Base/CE_SQLite.dat");
    if (!db.open()) {
        return false;
    }else{
        qDebug()<<"SQLite Ok";
        return true;
    }
}

void DataBaseDirection::closeDataBase()
{
    QSqlDatabase::removeDatabase("d:/apache/QtProject/ServerEmployee/Base/CE_SQLite.dat");
}
