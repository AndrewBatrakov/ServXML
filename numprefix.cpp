#include <QtSql>
#include <QtCore>

#include "numprefix.h"
//#include "prefixnumber.h"

NumPrefix::NumPrefix()
{
}

QString NumPrefix::getPrefix(QString tableName)
{
    QString returnString;

    QString  prefName= "SRV";

    QSqlQuery query;
    QString stringQuery = "SELECT ";
    stringQuery += tableName;
    stringQuery += "id FROM ";
    stringQuery += tableName;
    stringQuery += " WHERE ";
    stringQuery += tableName;
    stringQuery += "id LIKE '%";
    stringQuery += prefName;
    stringQuery += "%' ";
    stringQuery += " ORDER BY ";
    stringQuery += tableName;
    stringQuery += "id";
    //stringQuery.arg(prefName);
    query.exec(stringQuery);
    QString numberTemp, numberLast;

    //try insert record in empty row
    bool intoBool = false;
    int numNum = 1;
    while(query.next()){
        if(query.value(0).toString().right(9).toInt() > numNum){
            qDebug()<<numNum;
            numberLast = numNum;
            intoBool = true;
            break;
        }
        ++numNum;
    }

    if(!intoBool){
        query.last();
        numberLast = query.value(0).toString();
        if(numberLast == ""){
            numberLast = "0";
        }
        numberTemp = numberLast.right(9);
        numNum = numberTemp.toInt();
        ++numNum;
    }
    QString nTemp = QString::number(numNum);
    int www;
    www = nTemp.count();
    QString tt;
    for(int i = www; i < 9; ++i){
        tt += "0";
    }
    tt += QString::number(numNum);
    tt = prefName + tt;
    return tt;
}
