#include "readxml.h"
#include "numprefix.h"
#include "databasedirection.h"

ReadXML::ReadXML()
{
    QDir::setCurrent("d:/apache/QtProject/ServerEmployee/Change/");
    watcher = new QFileSystemWatcher;
    watcher->addPath("d:/apache/QtProject/ServerEmployee/Change/");
    connect(watcher,SIGNAL(directoryChanged(QString)),SLOT(updateDir(QString)));
    connect(watcher,SIGNAL(fileChanged(QString)),SLOT(updateDir(QString)));
    modifyBase = false;
}

void ReadXML::updateDir(QString dir)
{
    if(!modifyBase){
        QDir::setCurrent(dir);
        QFile file;
        file.setFileName("NullObmenAll.arh");
        if(file.exists()){
            QFile fileO;
            fileO.setFileName("ObmenAll.arh");
            fileO.copy("d:/apache/QtProject/ServerEmployee/Base/ObmenAll.arh");
            fileO.remove();
            file.remove();
            updateBase();
            modifyBase = false;
        }
    }
}

void ReadXML::updateBase()
{
    modifyBase = true;
    QDir::setCurrent("d:/apache/QtProject/ServerEmployee/Base/");
    QFile file;
    file.setFileName("ObmenAll.arh");
    if(file.exists()){
        file.open(QIODevice::ReadOnly);
        QByteArray byte = file.readAll();
        file.close();
        QByteArray unCompress = qUncompress(byte);
        QFile unCompressFile("ObmenAll.xml");
        unCompressFile.open(QIODevice::WriteOnly);
        unCompressFile.write(unCompress);
        unCompressFile.close();
        unCompressFile.open(QIODevice::ReadOnly);
        qDebug()<<unCompressFile.isOpen();

        DataBaseDirection db;
        db.connectDataBase();

        QDomDocument domDoc;
        if(domDoc.setContent(&unCompressFile)){
            QDomElement domElement = domDoc.documentElement();
            traverseAllInformation(domElement);
        }
        unCompressFile.close();

        QSqlQuery queryD;
        queryD.prepare("DELETE FROM employee WHERE fordelete = fd");
        queryD.bindValue(":fd",0);
        queryD.exec();
        queryD.next();
        //qDebug()<<queryD.next();

        db.closeDataBase();

        QFile fileOut;
        fileOut.setFileName("CE_SQLite.dat");
        fileOut.open(QIODevice::ReadOnly);
        QByteArray byteArray = fileOut.readAll();
        if(byteArray.isEmpty()){
            qDebug() << fileOut.errorString();
        }


        QByteArray compressData = qCompress(byteArray);
        QFile compressFile;
        QFileInfo fileInfo;
        fileInfo.setFile(fileOut.fileName());
        QString fN = fileInfo.baseName();
        fN += ".arh";
        compressFile.setFileName(fN);
        compressFile.open(QIODevice::WriteOnly);
        compressFile.write(compressData);

        fileOut.close();
        compressFile.close();

        QFile verifyFile("d:/apache/QtProject/ServerEmployee/Change/CE_SQLite.arh");
        if(verifyFile.exists()){
            verifyFile.remove();
        }
        fileOut.copy("d:/apache/QtProject/ServerEmployee/Change/CE_SQLite.arh");
    }
}

void ReadXML::traverseAllInformation(const QDomNode &node)
{
    //Only Employee INSERT And UPDATE
    QTextStream outC(stdout);
    outC.setCodec("IBM866");

    QDomNode domNode = node.firstChild();

    while(!domNode.isNull()){
        if(domNode.isElement()){
            QDomElement domElement = domNode.toElement();

            if(!domElement.isNull()){
                if(domElement.tagName() == "Element"){
                    int tabNumber;
                    tabNumber = domElement.attribute("TabelNumber","").simplified().toInt();
                    QString personName;
                    personName = domElement.attribute("Employee","").simplified();
                    QString upperName;
                    upperName = personName.toUpper();
                    QString orgName;
                    orgName = domElement.attribute("Organization","").simplified();
                    QString dateName;
                    dateName = domElement.attribute("DataRozdeniya","");

                    bool allData = false;
                    QString orgId, subId, postId, personId, grafikRabotId;
                    orgId = "OWN000000001";

                    QSqlQuery queryPer;
                    queryPer.prepare("SELECT employeeid FROM employee "
                                     "WHERE employeename = :personname AND "
                                     "datebirthday = :datebirth;");
                    queryPer.bindValue(":personname",personName);
                    //queryPer.bindValue(":orgid",orgId);
                    queryPer.bindValue(":datebirth",dateName);
                    queryPer.exec();
                    queryPer.next();
                    qDebug()<<queryPer.lastError().text();
                    if(queryPer.isValid()){
                        personId = queryPer.value(0).toString();
                        QSqlQuery queryUPDATE;
                        queryUPDATE.prepare("UPDATE employee SET "
                                            "fordelete = :fordelete, "
                                            "employeenameupper = :employeenameupper "
                                            "WHERE (employeeid = :personid);");
                        queryUPDATE.bindValue(":personid",personId);
                        queryUPDATE.bindValue(":employeenameupper",upperName);
                        queryUPDATE.bindValue(":fordelete",1);
                        queryUPDATE.exec();
                        if(!queryUPDATE.isActive()){
                            QString error = "passportDataVidachi";
                            error += ", ";
                            error += queryUPDATE.lastError().text();
                            error += " Employee UPDATE";
                            qDebug()<<error;
                        }
                        //************************************
                        //Need to add!!!
                        //if(QDate::currentDate().dayOfWeek() == 7){
                        //    allData = true;
                        //}

                    }else{

                        QString subName;
                        subName = domElement.attribute("Subdivision","").simplified();
                        QString postName;
                        postName = domElement.attribute("Post","").simplified();
                        QString kodKarty;
                        kodKarty = domElement.attribute("KodKarty","").simplified();
                        QString datePostupleniya;
                        datePostupleniya = domElement.attribute("DatePostupleniya","");//,"dd.MM.yyyy h:mm:ss").date();
                        QString grafikRabot;
                        grafikRabot = domElement.attribute("GrafikRabot","").simplified();
                        QString passportSeriya;
                        passportSeriya = domElement.attribute("PassportSeriya","").simplified();
                        QString passportNomer;
                        passportNomer = domElement.attribute("PassportNomer","").simplified();
                        QString passportDataVidachi;
                        passportDataVidachi = domElement.attribute("PassportDataVidachi","").simplified();
                        QString passportKemVidan;
                        passportKemVidan = domElement.attribute("KemVidan","").simplified();

                        int rost, odezhdaZim, odezhdaLet, obuvZim, obuvLet, golUbor;
                        rost = domElement.attribute("Rost","").toInt();
                        odezhdaZim = domElement.attribute("OdezhdaZim","").toInt();
                        odezhdaLet = domElement.attribute("OdezhdaLet","").toInt();
                        obuvZim = domElement.attribute("ObuvZim","").toInt();
                        obuvLet = domElement.attribute("ObuvLet","").toInt();
                        golUbor = domElement.attribute("GolUbor","").toInt();

                        QString uchebnoeZavedenie[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "UchebnoeZavedenie";
                            domString += QString::number(i);
                            uchebnoeZavedenie[i] = domElement.attribute(domString,"").simplified();
                            if(uchebnoeZavedenie[i].isEmpty())
                                break;

                        }
                        QString spezialnost[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "Spezialnost";
                            domString += QString::number(i);
                            spezialnost[i] = domElement.attribute(domString,"").simplified();
                            if(spezialnost[i].isEmpty())
                                break;
                        }

                        QString diplom[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "Diplom";
                            domString += QString::number(i);
                            diplom[i] = domElement.attribute(domString,"").simplified();
                            if(diplom[i].isEmpty())
                                break;
                        }
                        QString kvalificaziya[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "Kvalificaziya";
                            domString += QString::number(i);
                            kvalificaziya[i] = domElement.attribute(domString,"").simplified();
                            if(kvalificaziya[i].isEmpty())
                                break;
                        }

                        QString godOconchaniy[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "GodOconchaniy";
                            domString += QString::number(i);
                            godOconchaniy[i] = domElement.attribute(domString,"").simplified();
                            if(godOconchaniy[i].isEmpty())
                                break;
                        }

                        QString vidObrazovaniya[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "VidObrazovaniya";
                            domString += QString::number(i);
                            vidObrazovaniya[i] = domElement.attribute(domString,"").simplified();
                            if(vidObrazovaniya[i].isEmpty())
                                break;
                        }

                        QString tephone[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "Tephone";
                            domString += QString::number(i);
                            tephone[i] = domElement.attribute(domString,"").simplified();
                            if(tephone[i].isEmpty())
                                break;
                        }
                        QString stepenRodstva[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "StepenRodstva";
                            domString += QString::number(i);
                            stepenRodstva[i] = domElement.attribute(domString,"").simplified();
                            if(stepenRodstva[i].isEmpty())
                                break;
                        }

                        QString nameRodstva[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "NameRodstva";
                            domString += QString::number(i);
                            nameRodstva[i] = domElement.attribute(domString,"").simplified();
                            if(nameRodstva[i].isEmpty())
                                break;
                        }

                        QString dateRodstva[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "DateRodstva";
                            domString += QString::number(i);
                            dateRodstva[i] = domElement.attribute(domString,"").simplified();
                            if(dateRodstva[i].isEmpty())
                                break;
                        }

                        QString sizName[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "sizName";
                            domString += QString::number(i);
                            sizName[i] = domElement.attribute(domString,"").simplified();
                            if(sizName[i].isEmpty())
                                break;
                        }

                        QString sizOstatok[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "sizOstatok";
                            domString += QString::number(i);
                            sizOstatok[i] = domElement.attribute(domString,"").simplified();
                            if(sizOstatok[i].isEmpty())
                                break;
                        }

                        QString sizData[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "sizData";
                            domString += QString::number(i);
                            sizData[i] = domElement.attribute(domString,"").simplified();
                            if(sizData[i].isEmpty())
                                break;
                        }

                        QString sizSrok[30];
                        for(int i = 1; i < 30; ++i){
                            QString domString = "sizSrok";
                            domString += QString::number(i);
                            sizSrok[i] = domElement.attribute(domString,"").simplified();
                            if(sizData[i].isEmpty())
                                break;
                        }

                        QString mNomerSpravki;
                        mNomerSpravki = domElement.attribute("mNomerSpravki","").simplified();
                        QString mNomerDocumenta;
                        mNomerDocumenta = domElement.attribute("mNomerDocumenta","").simplified();
                        QString mData;
                        mData = domElement.attribute("mData","").simplified();
                        QString mDataProhojdeniya;
                        mDataProhojdeniya = domElement.attribute("mDataProhojdeniya","").simplified();
                        int mProyden;
                        mProyden = domElement.attribute("mProyden","").toInt();
                        int mGoden;
                        mGoden = domElement.attribute("mGoden","").toInt();

                        QString pbNomer;
                        pbNomer = domElement.attribute("pbNomer","").simplified();
                        QString pbData;
                        pbData = domElement.attribute("pbData","").simplified();
                        QString pbObChasov;
                        pbObChasov = domElement.attribute("pbObChasov","").simplified();
                        QString pbProgrammaObucheniya;
                        pbProgrammaObucheniya = domElement.attribute("pbProgrammaObucheniya","").simplified();

                        QString otDataUdostovereniya;
                        otDataUdostovereniya = domElement.attribute("otDataUdostovereniya","").simplified();
                        QString otProtocolProgrammaObucheniya;
                        otProtocolProgrammaObucheniya = domElement.attribute("otProtocolProgrammaObucheniya","").simplified();
                        QString otProtocolNomer;
                        otProtocolNomer = domElement.attribute("otProtocolNomer","").simplified();
                        QString otProtokolData;
                        otProtokolData = domElement.attribute("otProtokolData","").simplified();
                        QString otProtokolObChasov;
                        otProtokolObChasov = domElement.attribute("otProtokolObChasov","").simplified();

                        QString ptmNumber;
                        ptmNumber = domElement.attribute("ptmNumber","").simplified();
                        QString ptmData;
                        ptmData = domElement.attribute("ptmData","").simplified();
                        QString ptmPTMNumber;
                        ptmPTMNumber = domElement.attribute("ptmPTMNumber","").simplified();
                        QString ptmPTMObChasov;
                        ptmPTMObChasov = domElement.attribute("ptmPTMObChasov","").simplified();
                        QString ptmPTMSostav;
                        ptmPTMSostav = domElement.attribute("ptmPTMSostav","").simplified();
                        QString ptmPTMProgramma;
                        ptmPTMProgramma = domElement.attribute("ptmPTMProgramma","").simplified();
                        QString ptmPTMDate;
                        ptmPTMDate = domElement.attribute("ptmPTMDate","").simplified();

                        QString numberUd;
                        numberUd = domElement.attribute("NumberUd","").simplified();
                        QString dataUd;
                        dataUd = domElement.attribute("DataUd","").simplified();
                        QString visNumber;
                        visNumber = domElement.attribute("visNumber").simplified();
                        QString visData;
                        visData = domElement.attribute("visData","").simplified();
                        QString visObChasov;
                        visObChasov = domElement.attribute("visObChasov","").simplified();

                        for(int i = 0; i < 30; ++i){
                            QString domString = "UchebnoeZavedenie";
                            domString += QString::number(i);
                            uchebnoeZavedenie[i] = domElement.attribute(domString,"").simplified();
                            if(uchebnoeZavedenie[i].isEmpty())
                                break;
                        }
                        for(int i = 0; i < 30; ++i){
                            QString domString = "Spezialnost";
                            domString += QString::number(i);
                            spezialnost[i] = domElement.attribute(domString,"").simplified();
                            if(spezialnost[i].isEmpty())
                                break;
                        }
                        for(int i = 0; i < 30; ++i){
                            QString domString = "Diplom";
                            domString += QString::number(i);
                            diplom[i] = domElement.attribute(domString,"").simplified();
                            if(diplom[i].isEmpty())
                                break;
                        }
                        for(int i = 0; i < 30; ++i){
                            QString domString = "Kvalificaziya";
                            domString += QString::number(i);
                            kvalificaziya[i] = domElement.attribute(domString,"").simplified();
                            if(kvalificaziya[i].isEmpty())
                                break;
                        }
                        for(int i = 0; i < 30; ++i){
                            QString domString = "GodOconchaniy";
                            domString += QString::number(i);
                            godOconchaniy[i] = domElement.attribute(domString,"").simplified();
                            if(godOconchaniy[i].isEmpty())
                                break;
                        }

                        //Subdivision
                        if(subName != ""){
                            QSqlQuery querySub;
                            querySub.prepare("SELECT subdivisionid FROM subdivision WHERE subdivisionname = :subname AND organizationid = :orgid;");
                            querySub.bindValue(":subname",subName);
                            querySub.bindValue(":orgid",orgId);
                            querySub.exec();
                            querySub.next();
                            if(!querySub.isValid()){
                                QSqlQuery query;
                                NumPrefix numPrefix;
                                QString idSub = numPrefix.getPrefix("subdivision");
                                query.prepare("INSERT INTO subdivision (subdivisionid, subdivisionname, organizationid) VALUES(:idsub, :namesub, :orgid);");
                                query.bindValue(":namesub",subName);
                                query.bindValue(":orgid",orgId);
                                query.bindValue(":idsub",idSub);
                                query.exec();
                                subId = idSub;
                            }else{
                                subId = querySub.value(0).toString();
                            }
                        }else{
                            continue;
                        }

                        //Post
                        if(postName != ""){
                            QSqlQuery queryPost;
                            queryPost.prepare("SELECT postid FROM post WHERE postname = :postname;");
                            queryPost.bindValue(":postname",postName);
                            queryPost.exec();
                            queryPost.next();
                            if(!queryPost.isValid()){
                                NumPrefix numPrefix;
                                QString idPost = numPrefix.getPrefix("post");
                                QSqlQuery query;
                                query.prepare("INSERT INTO post (postid, postname) VALUES(:idpost, :namepost);");
                                query.bindValue(":namepost",postName);
                                query.bindValue(":idpost", idPost);
                                query.exec();
                                postId = idPost;
                            }else{
                                postId = queryPost.value(0).toString();
                            }
                        }else{
                            continue;
                        }

                        //Grafik Rabot
                        if(grafikRabot != ""){
                            QSqlQuery queryPost;
                            queryPost.prepare("SELECT grafikrabotid FROM grafikrabot WHERE grafikrabotname = :grafikrabotname;");
                            queryPost.bindValue(":grafikrabotname",grafikRabot);
                            queryPost.exec();
                            queryPost.next();
                            if(!queryPost.isValid()){
                                NumPrefix numPrefix;
                                QString idGrafik = numPrefix.getPrefix("grafikrabot");
                                QSqlQuery query;
                                query.prepare("INSERT INTO grafikrabot (grafikrabotid, grafikrabotname) "
                                              "VALUES(:grafikrabotid, :grafikrabotname);");
                                query.bindValue(":grafikrabotname",grafikRabot);
                                query.bindValue(":grafikrabotid", idGrafik);
                                query.exec();
                                grafikRabotId = idGrafik;
                            }else{
                                grafikRabotId = queryPost.value(0).toString();
                            }
                        }

                        //************************************************************


                        //                    QSqlQuery queryPer;
                        //                    queryPer.prepare("SELECT employeeid FROM employee "
                        //                                     "WHERE employeename = :personname AND "
                        //                                     "datebirthday = :datebirth;");
                        //                    queryPer.bindValue(":personname",personName);
                        //                    queryPer.bindValue(":datebirth",dateName);
                        //                    queryPer.exec();
                        //                    queryPer.next();

                        allData = true;
                        QSqlQuery queryINSERT;
                        NumPrefix numPrefix;
                        personId = numPrefix.getPrefix("employee");
                        queryINSERT.prepare("INSERT INTO employee ("
                                            "employeeid, "
                                            "employeename, "
                                            "employeenameupper, "
                                            "organizationid, "
                                            "subdivisionid, "
                                            "postid, "
                                            "tabnumber, "
                                            "kodkarty, "
                                            "datebirthday, "
                                            "withorganization, "
                                            "passportseriya, "
                                            "passportnumber, "
                                            "passportissueddate, "
                                            "passportissuedby, "
                                            "grafikrabot, "
                                            "fordelete"
                                            ") VALUES("
                                            ":employeeid, "
                                            ":employeename, "
                                            ":organizationid, "
                                            ":subdivisionid, "
                                            ":postid, "
                                            ":tabnumber, "
                                            ":kodkarty, "
                                            ":datebirthday, "
                                            ":withorganization, "
                                            ":passportseriya, "
                                            ":passportnumber, "
                                            ":passportissueddate, "
                                            ":passportissuedby, "
                                            ":grafikrabot, "
                                            ":fordelete"
                                            ");");
                        queryINSERT.bindValue(":employeeid",personId);
                        queryINSERT.bindValue(":employeename",personName);
                        queryINSERT.bindValue(":employeenameupper",upperName);
                        queryINSERT.bindValue(":organizationid",orgId);
                        queryINSERT.bindValue(":subdivisionid",subId);
                        queryINSERT.bindValue(":postid", postId);
                        queryINSERT.bindValue(":tabnumber", tabNumber);
                        queryINSERT.bindValue(":kodkarty",kodKarty);
                        queryINSERT.bindValue(":datebirthday",dateName);
                        queryINSERT.bindValue(":withorganization",datePostupleniya);
                        queryINSERT.bindValue(":passportseriya",passportSeriya);
                        queryINSERT.bindValue(":passportnumber",passportNomer);
                        queryINSERT.bindValue(":passportissueddate",passportDataVidachi);
                        queryINSERT.bindValue(":passportissuedby",passportKemVidan);
                        queryINSERT.bindValue(":grafikrabot",grafikRabotId);
                        queryINSERT.bindValue(":fordelete",1);
                        queryINSERT.exec();
                        outC << personName << " - " << "Insert" <<endl;
                        if(!queryINSERT.isActive()){
                            QString error = personName;
                            error += ", ";
                            error += personId;
                            error += ", ";
                            error += queryINSERT.lastError().text();
                            error += "  Employee INSERT";
                            qDebug()<<error;
                        }

                        if(allData){
                            QSqlQuery queryUPDATE;
                            queryUPDATE.prepare("UPDATE employee SET "
                                                "kodkarty = :kodkart, "
                                                "tabnumber = :tabnumber, "
                                                "subdivisionid = :subid, "
                                                "postid = :postid, "
                                                "grafikrabot = :grafikrabot, "
                                                "passportseriya = :passportseriya, "
                                                "passportnumber = :passportnumber, "
                                                "passportissueddate = :passportissueddate, "
                                                "withorganization = :with, "
                                                "passportissuedby = :issue, "
                                                "fordelete = :fordelete "
                                                "WHERE (employeeid = :personid);");
                            queryUPDATE.bindValue(":personid",personId);
                            queryUPDATE.bindValue(":tabnumber",tabNumber);
                            queryUPDATE.bindValue(":kodkart",kodKarty);
                            queryUPDATE.bindValue(":subid",subId);
                            queryUPDATE.bindValue(":postid", postId);
                            queryUPDATE.bindValue(":grafikrabot",grafikRabotId);
                            queryUPDATE.bindValue(":passportseriya",passportSeriya);
                            queryUPDATE.bindValue(":passportnumber",passportNomer);
                            queryUPDATE.bindValue(":passportissueddate",passportDataVidachi);
                            queryUPDATE.bindValue(":with",datePostupleniya);
                            queryUPDATE.bindValue(":issue",passportKemVidan);
                            queryUPDATE.bindValue(":fordelete",1);
                            queryUPDATE.exec();
                            if(!queryUPDATE.isActive()){
                                QString error = passportDataVidachi;
                                error += ", ";
                                error += queryUPDATE.lastError().text();
                                qDebug()<<"Employee UPDATE "<<error;
                            }

                            //Telephone
                            for(int i = 1; i < 30; ++i){
                                if(tephone[i] != ""){
                                    QSqlQuery queryOrg;
                                    queryOrg.prepare("SELECT * FROM personalinformation WHERE (employeeid = :id AND information = :inf);");
                                    queryOrg.bindValue(":id",personId);
                                    queryOrg.bindValue(":inf",tephone[i]);
                                    queryOrg.exec();
                                    queryOrg.next();
                                    if(!queryOrg.isValid()){
                                        QSqlQuery query;
                                        NumPrefix numPrefix;
                                        QString idTe = numPrefix.getPrefix("personalinformation");
                                        query.prepare("INSERT INTO personalinformation ("
                                                      "personalinformationid, "
                                                      "employeeid, "
                                                      "information"
                                                      ") VALUES(:perid, :empid, :information);");
                                        query.bindValue(":perid",idTe);
                                        query.bindValue(":empid",personId);
                                        query.bindValue(":information",tephone[i]);
                                        query.exec();

                                        if(!query.isActive()){
                                            QString error = passportDataVidachi;
                                            error += ", ";
                                            error += query.lastError().text();
                                            qDebug()<<"Telephone INSERT "<<error;
                                        }
                                    }
                                }
                            }
                            //Education
                            for(int i = 1; i < 30; ++i){
                                if(vidObrazovaniya[i] != ""){
                                    QSqlQuery queryOrg;
                                    queryOrg.prepare("SELECT * FROM education WHERE (employeeid = :id AND typeofeducation = :type);");
                                    queryOrg.bindValue(":id",personId);
                                    queryOrg.bindValue(":type",vidObrazovaniya[i]);
                                    queryOrg.exec();
                                    queryOrg.next();
                                    if(!queryOrg.isValid()){
                                        QSqlQuery query;
                                        NumPrefix numPrefix;
                                        QString idEdu = numPrefix.getPrefix("education");
                                        query.prepare("INSERT INTO education ("
                                                      "educationid, "
                                                      "employeeid, "
                                                      "typeofeducation, "
                                                      "educationalinstitution, "
                                                      "specialty, "
                                                      "diplom, "
                                                      "year, "
                                                      "qualification"
                                                      ") VALUES(:edid, :empid, :typeofeducation,"
                                                      ":educationalinstitution, :specialty, :diplom, :year, :qualification);");
                                        query.bindValue(":edid",idEdu);
                                        query.bindValue(":empid",personId);
                                        query.bindValue(":typeofeducation",vidObrazovaniya[i]);
                                        query.bindValue(":educationalinstitution",uchebnoeZavedenie[i]);
                                        query.bindValue(":specialty",spezialnost[i]);
                                        query.bindValue(":diplom",diplom[i]);
                                        query.bindValue(":year",godOconchaniy[i]);
                                        query.bindValue(":qualification",kvalificaziya[i]);
                                        query.exec();
                                        if(!query.isActive()){
                                            QString error = passportDataVidachi;
                                            error += ", ";
                                            error += query.lastError().text();
                                            qDebug()<<"Education INSERT "<<error;
                                        }
                                    }
                                }
                            }
                            if(mNomerDocumenta != "" && mData != ""){
                                QSqlQuery queryMed;
                                queryMed.prepare("SELECT * FROM physical WHERE (employeeid  = :id "
                                                 "AND datedirection = :datedir);");
                                queryMed.bindValue(":id",personId);
                                queryMed.bindValue(":datdir",mData);
                                queryMed.exec();
                                queryMed.next();
                                if(!queryMed.isValid()){
                                    //outC<<medosmotrNomerDocumenta<<medosmotrData<<personId<<endl;
                                    QSqlQuery query;
                                    NumPrefix numPrefix;
                                    QString idMed = numPrefix.getPrefix("physical");
                                    query.prepare("INSERT INTO physical ("
                                                  "physicalid, "
                                                  "employeeid, "
                                                  "numberdirection, "
                                                  "datedirection, "
                                                  "passed, "
                                                  "fit, "
                                                  "dateofpassage, "
                                                  "numberspravki"
                                                  ") VALUES("
                                                  ":id, :empid, :numdir, :datdir, :pas, :fit, :datpass, :nomsprav);");
                                    query.bindValue(":id",idMed);
                                    query.bindValue(":empid",personId);
                                    query.bindValue(":numdir",mNomerDocumenta);
                                    query.bindValue(":datdir",mData);
                                    query.bindValue(":pas",mProyden);
                                    query.bindValue(":fit",mGoden);
                                    if(mDataProhojdeniya == ""){
                                        mDataProhojdeniya = "1900-01-01T00:00:00";
                                    }
                                    query.bindValue(":datpass",mDataProhojdeniya);
                                    query.bindValue(":nomsprav",mNomerSpravki);
                                    query.exec();
                                    if(!query.isActive()){
                                        qDebug()<<"medosmotr insert error "<<query.lastError().text();
                                    }
                                }
                            }
                            if(pbNomer != "" && pbData != ""){
                                QSqlQuery queryPB;
                                queryPB.prepare("SELECT * FROM industrialsecurity WHERE (employeeid  = :id "
                                                "AND industrialsecuritydate = :datdir);");
                                queryPB.bindValue(":id",personId);
                                queryPB.bindValue(":datdir",pbData);
                                queryPB.exec();
                                queryPB.next();
                                if(!queryPB.isValid()){
                                    QString pbProgrammaid;
                                    if(pbProgrammaObucheniya != ""){
                                        QSqlQuery queryI;
                                        queryI.prepare("SELECT * FROM pbprogramma WHERE pbprogrammaname = :pbprogrammaname");
                                        queryI.bindValue(":pbprogrammaname",pbProgrammaObucheniya);
                                        queryI.exec();
                                        queryI.next();
                                        if(!queryI.isValid()){
                                            QSqlQuery query;
                                            NumPrefix numPrefix;
                                            pbProgrammaid = numPrefix.getPrefix("pbprogramma");
                                            query.prepare("INSERT INTO pbprogramma ("
                                                          "pbprogrammaid, "
                                                          "pbprogrammaname"
                                                          ") VALUES(:pbprogrammaid, :pbprogrammaname);");
                                            query.bindValue(":pbprogrammaid",pbProgrammaid);
                                            query.bindValue(":pbprogrammaname",pbProgrammaObucheniya);
                                            query.exec();
                                        }else{
                                            pbProgrammaid = queryI.value(0).toString();
                                        }
                                    }else{
                                        pbProgrammaid = "";
                                    }
                                    QSqlQuery query;
                                    NumPrefix numPrefix;
                                    QString idPB = numPrefix.getPrefix("industrialsecurity");
                                    query.prepare("INSERT INTO industrialsecurity ("
                                                  "industrialsecurityid, "
                                                  "employeeid, "
                                                  "industrialsecuritydate, "
                                                  "trainingprogrammid, "
                                                  "industrialsecuritynumber, "
                                                  "obchasov"
                                                  ") VALUES("
                                                  ":id, :empid, :datdir, :progid, :fit, :obch);");
                                    query.bindValue(":id",idPB);
                                    query.bindValue(":empid",personId);
                                    query.bindValue(":datdir",pbData);
                                    query.bindValue(":progid",pbProgrammaid);
                                    query.bindValue(":fit",pbNomer);
                                    query.bindValue(":obch",pbObChasov);
                                    query.exec();
                                    if(!query.isActive()){
                                        qDebug()<<"Industrial security "<<query.lastError().text();
                                    }
                                }
                            }
                            if(otProtocolNomer != "" && otProtokolData != ""){
                                QSqlQuery queryOT;
                                queryOT.prepare("SELECT * FROM laborprotection WHERE (employeeid  = :id "
                                                "AND laborprotectiondate = :datprot);");
                                queryOT.bindValue(":id",personId);
                                queryOT.bindValue(":datprot",otProtokolData);
                                queryOT.exec();
                                queryOT.next();
                                if(!queryOT.isValid()){
                                    QString otProgrammaid;
                                    if(otProtocolProgrammaObucheniya != ""){
                                        QSqlQuery queryO;
                                        queryO.prepare("SELECT * FROM otprogramma WHERE otprogrammaname = :otprogrammaname");
                                        queryO.bindValue(":otprogrammaname",otProtocolProgrammaObucheniya);
                                        queryO.exec();
                                        queryO.next();
                                        if(!queryO.isValid()){
                                            QSqlQuery query;
                                            NumPrefix numPrefix;
                                            otProgrammaid = numPrefix.getPrefix("otprogramma");
                                            query.prepare("INSERT INTO otProgramma ("
                                                          "otprogrammaid, "
                                                          "otprogrammaname"
                                                          ") VALUES(:otprogrammaid, :otprogrammaname);");
                                            query.bindValue(":otprogrammaid",otProgrammaid);
                                            query.bindValue(":otprogrammaname",otProtocolProgrammaObucheniya);
                                            query.exec();
                                        }else{
                                            otProgrammaid = queryO.value(0).toString();
                                        }
                                    }else{
                                        otProgrammaid = "";
                                    }
                                    QSqlQuery query;
                                    NumPrefix numPrefix;
                                    QString idProt = numPrefix.getPrefix("laborprotection");
                                    query.prepare("INSERT INTO laborprotection ("
                                                  "laborprotectionid, "
                                                  "employeeid, "
                                                  "laborprotectiondate, "
                                                  "trainingprogrammid, "
                                                  "laborprotectionnumber, "
                                                  "laborprotectionudostoverenie"
                                                  ") VALUES("
                                                  ":id, :empid, :datprot, :programmid, :labprotnum, :labprotudost);");
                                    query.bindValue(":id",idProt);
                                    query.bindValue(":empid",personId);
                                    query.bindValue(":datprot",otProtokolData);
                                    query.bindValue(":programmid",otProgrammaid);
                                    query.bindValue(":labprotnum",otProtocolNomer);
                                    query.bindValue(":labprotudost",otDataUdostovereniya);
                                    query.exec();
                                    if(!query.isActive()){
                                        qDebug()<<"Ohrana truda"<<query.lastError().text();
                                    }
                                }
                            }
                            if(ptmPTMNumber != "" && ptmPTMDate != ""){
                                QSqlQuery queryPTM;
                                queryPTM.prepare("SELECT * FROM firesafety WHERE (employeeid  = :id AND firesafetynumber = :numfire "
                                                 "AND firesafetydate = :datfire);");
                                queryPTM.bindValue(":id",personId);
                                queryPTM.bindValue(":datfire",ptmPTMDate);
                                queryPTM.bindValue(":numfire",ptmNumber);
                                queryPTM.exec();
                                queryPTM.next();
                                if(!queryPTM.isValid()){
                                    QString firetrainingid;
                                    if(ptmPTMProgramma != ""){
                                        QSqlQuery queryO;
                                        queryO.prepare("SELECT * FROM ptmprogramma WHERE ptmprogrammaname = :ptmprogrammaname");
                                        queryO.bindValue(":ptmprogrammaname",ptmPTMProgramma);
                                        queryO.exec();
                                        queryO.next();

                                        if(!queryO.isValid()){
                                            QSqlQuery query;
                                            NumPrefix numPrefix;
                                            firetrainingid = numPrefix.getPrefix("ptmprogramma");
                                            query.prepare("INSERT INTO ptmprogramma ("
                                                          "ptmprogrammaid, "
                                                          "ptmprogrammaname"
                                                          ") VALUES(:ptmprogrammaid, :ptmprogrammaname);");
                                            query.bindValue(":ptmprogrammaid",firetrainingid);
                                            query.bindValue(":ptmprogrammaname",ptmPTMProgramma);
                                            query.exec();
                                        }else{
                                            firetrainingid = queryO.value(0).toString();
                                        }
                                    }else{
                                        firetrainingid = "";
                                    }
                                    QSqlQuery query;
                                    NumPrefix numPrefix;
                                    QString idFire = numPrefix.getPrefix("firesafety");
                                    query.prepare("INSERT INTO firesafety ("
                                                  "firesafetyid, "
                                                  "employeeid, "
                                                  "firesafetydate, "
                                                  "firesafetynumber, "
                                                  "firesafetyhour, "
                                                  "firesafetysostav, "
                                                  "firesafetyudnumber, "
                                                  "firesafetyuddate, "
                                                  "firetrainingid"
                                                  ") VALUES("
                                                  ":id, :empid, :datfire, :numfire, :hourfire, :sostav, "
                                                  ":udnumber, :uddate, :firetrainingid);");
                                    query.bindValue(":id",idFire);
                                    query.bindValue(":empid",personId);
                                    query.bindValue(":datfire",ptmPTMDate);
                                    query.bindValue(":numfire",ptmPTMNumber);
                                    query.bindValue(":hourfire",ptmPTMObChasov);
                                    query.bindValue(":sostav",ptmPTMSostav);
                                    query.bindValue(":udnumber",ptmNumber);
                                    query.bindValue("firetrainingid",firetrainingid);
                                    if(ptmData == ""){
                                        ptmData = "1900:01:01T00:00:00";
                                    }
                                    query.bindValue(":uddate",ptmData);
                                    query.exec();
                                    if(!query.isActive()){
                                        qDebug()<<"Ptm Error"<<query.lastError().text();
                                    }
                                }
                            }
                            if(rost != 0){
                                QSqlQuery queryOv;
                                queryOv.prepare("SELECT * FROM overal WHERE employeeid  = :id;");
                                queryOv.bindValue(":id",personId);
                                queryOv.exec();
                                queryOv.next();
                                if(!queryOv.isValid()){
                                    QSqlQuery query;
                                    NumPrefix numPrefix;
                                    QString idOveral = numPrefix.getPrefix("overal");
                                    query.prepare("INSERT INTO overal ("
                                                  "overalid, "
                                                  "employeeid, "
                                                  "growth, "
                                                  "headdress, "
                                                  "winclothes, "
                                                  "sumclothes, "
                                                  "winshoes, "
                                                  "sumshoes"
                                                  ") VALUES("
                                                  ":id, :empid, :growth, :headdress, :winclothes, :sumclothes, :winshoes, :sumshoes);");
                                    query.bindValue(":id",idOveral);
                                    query.bindValue(":empid",personId);
                                    query.bindValue(":growth",rost);
                                    query.bindValue(":headdress",golUbor);
                                    query.bindValue(":winclothes",odezhdaZim);
                                    query.bindValue(":sumclothes",odezhdaLet);
                                    query.bindValue(":winshoes",obuvZim);
                                    query.bindValue(":sumshoes",obuvLet);
                                    query.exec();
                                    if(!query.isActive()){
                                        qDebug()<<"Overal ERROR!"<<query.lastError().text();
                                    }
                                }
                            }
                            for(int i = 1; i < 30; ++i){
                                if(stepenRodstva[i] != ""){
                                    QSqlQuery queryOrg;
                                    queryOrg.prepare("SELECT * FROM rodnie WHERE (employeeid = :id AND stepenrodstva = :stepen "
                                                     "AND namerodstva = :name);");
                                    queryOrg.bindValue(":id",personId);
                                    queryOrg.bindValue(":stepen",stepenRodstva[i]);
                                    queryOrg.bindValue(":name",nameRodstva[i]);
                                    queryOrg.exec();
                                    queryOrg.next();
                                    if(!queryOrg.isValid()){
                                        QSqlQuery query;
                                        NumPrefix numPrefix;
                                        QString idEdu = numPrefix.getPrefix("rodnie");
                                        query.prepare("INSERT INTO rodnie ("
                                                      "rodnieid, "
                                                      "employeeid, "
                                                      "stepenrodstva, "
                                                      "namerodstva, "
                                                      "birthrodstva"
                                                      ") VALUES(:edid, :empid, :stepen,"
                                                      ":name, :birth);");
                                        query.bindValue(":edid",idEdu);
                                        query.bindValue(":empid",personId);
                                        query.bindValue(":stepen",stepenRodstva[i]);
                                        query.bindValue(":name",nameRodstva[i]);
                                        query.bindValue(":birth",dateRodstva[i]);
                                        query.exec();
                                        if(!query.isActive()){
                                            qDebug()<<"Rodstva ERROR!"<<query.lastError().text();
                                        }
                                    }
                                }
                            }
                            for(int i = 1; i < 30; ++i){
                                if(sizName[i] != ""){
                                    QSqlQuery querySIZ;
                                    querySIZ.prepare("SELECT * FROM siznaim WHERE siznaimname = :siznaimname");
                                    querySIZ.bindValue(":siznaimname",sizName[i]);
                                    querySIZ.exec();
                                    querySIZ.next();
                                    QString idSizNaim;
                                    if(!querySIZ.isValid()){
                                        QSqlQuery query;
                                        NumPrefix numPrefix;
                                        idSizNaim = numPrefix.getPrefix("siznaim");
                                        query.prepare("INSERT INTO siznaim ("
                                                      "siznaimid, "
                                                      "siznaimname"
                                                      ") VALUES(:siznaimid, :siznaimname);");
                                        query.bindValue(":siznaimid",idSizNaim);
                                        query.bindValue(":siznaimname",sizName[i]);
                                        query.exec();
                                    }else{
                                        idSizNaim = querySIZ.value(0).toString();
                                    }
                                    QSqlQuery queryOrg;
                                    queryOrg.prepare("SELECT * FROM siz WHERE (employeeid = :id AND siznaim = :siznaim);");
                                    queryOrg.bindValue(":id",personId);
                                    queryOrg.bindValue(":siznaim",idSizNaim);
                                    queryOrg.exec();
                                    queryOrg.next();
                                    if(!queryOrg.isValid()){
                                        QSqlQuery query;
                                        NumPrefix numPrefix;
                                        QString idSiz = numPrefix.getPrefix("siz");
                                        query.prepare("INSERT INTO siz ("
                                                      "sizid, "
                                                      "employeeid, "
                                                      "siznaimid, "
                                                      "ostatok, "
                                                      "dataperedachi, "
                                                      "srokisp"
                                                      ") VALUES(:sizid, :employeeid, :siznaimid, :ostatok, "
                                                      ":dataperedachi, :srokisp);");
                                        query.bindValue(":employeeid",personId);
                                        query.bindValue(":sizid",idSiz);
                                        query.bindValue(":siznaimid",idSizNaim);
                                        query.bindValue(":ostatok",sizOstatok[i]);
                                        query.bindValue(":dataperedachi",sizData[i]);
                                        query.bindValue(":srokisp",sizSrok[i]);
                                        query.exec();
                                        query.next();
                                        if(!query.isActive()){
                                            qDebug()<<"SIZ ERROR!"<<query.lastError().text();
                                        }
                                    }
                                }
                            }
                            if(visNumber != "" && visData != ""){
                                QSqlQuery queryVIS;
                                queryVIS.prepare("SELECT * FROM visota WHERE (employeeid  = :id AND visnumber = :visnumber "
                                                 "AND visdate = :visdate);");
                                queryVIS.bindValue(":id",personId);
                                queryVIS.bindValue(":visdate",visData);
                                queryVIS.bindValue(":visnumber",visNumber);
                                queryVIS.exec();
                                queryVIS.next();
                                if(!queryVIS.isValid()){
                                    QSqlQuery query;
                                    NumPrefix numPrefix;
                                    QString idVIS = numPrefix.getPrefix("visota");
                                    query.prepare("INSERT INTO visota ("
                                                  "visid, "
                                                  "employeeid, "
                                                  "visdate, "
                                                  "visnumber, "
                                                  "visobchasov, "
                                                  "numberud, "
                                                  "dateud"
                                                  ") VALUES("
                                                  ":visid, :employeeid, :visdate, :visnumber, :visobchasov, :numberud, "
                                                  ":dateud);");
                                    query.bindValue(":visid",idVIS);
                                    query.bindValue(":employeeid",personId);
                                    query.bindValue(":visdate",visData);
                                    query.bindValue(":visnumber",visNumber);
                                    query.bindValue(":visobchasov",visObChasov);
                                    query.bindValue(":numberud",numberUd);
                                    query.bindValue(":dateud",dataUd);
                                    query.exec();
                                    if(!query.isActive()){
                                        qDebug()<<"Visota Error"<<query.lastError().text();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            traverseAllInformation(domNode);
            domNode = domNode.nextSibling();
        }
    }
}
