#ifndef DATABASEDIRECTION_H
#define DATABASEDIRECTION_H

#include <QtSql>

class DataBaseDirection : public QObject
{
    Q_OBJECT
public:
    DataBaseDirection();

public slots:
    bool connectDataBase();
    void closeDataBase();

};
#endif // DATABASEDIRECTION_H
