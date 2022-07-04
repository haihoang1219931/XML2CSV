#include <stdio.h>
#include <cstdio>
#include <tinyxml2.h>
#include <FileControler.h>
#include <QObject>
#include <QVariantMap>

QVariantMap getContactInfo(tinyxml2::XMLElement* contact);
QVariantMap getContactNumber(tinyxml2::XMLElement* contactSubItem);
void gotoxy(int x,int y);

int main(int argc, char *argv[])
{
    printf("Converting databse\r\n");
    if(argc < 3)
    {
        printf("%s [input(.list)] [output file(.csv)]\r\n",argv[0]);
        return -1;
    }
    tinyxml2::XMLDocument doc;
    doc.LoadFile(argv[1]);
    tinyxml2::XMLElement* list = doc.FirstChildElement("plist")->FirstChildElement("dict")->FirstChildElement("array");
    std::string outputFile(argv[2]);
    std::ofstream ofs;
    ofs.open(outputFile, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    FileController::addLine(outputFile,"Name,Given Name,Additional Name,Family Name,Yomi Name,Given Name Yomi,Additional Name Yomi,Family Name Yomi,Name Prefix,Name Suffix,Initials,Nickname,Short Name,Maiden Name,Birthday,Gender,Location,Billing Information,Directory Server,Mileage,Occupation,Hobby,Sensitivity,Priority,Subject,Notes,Language,Photo,Group Membership,E-mail 1 - Type,E-mail 1 - Value,E-mail 2 - Type,E-mail 2 - Value,Phone 1 - Type,Phone 1 - Value,Phone 2 - Type,Phone 2 - Value,Organization 1 - Type,Organization 1 - Name,Organization 1 - Yomi Name,Organization 1 - Title,Organization 1 - Department,Organization 1 - Symbol,Organization 1 - Location,Organization 1 - Job Description");
    if(list != nullptr)
    {
        int count = 0;
        for(tinyxml2::XMLElement* contact = list->FirstChildElement(); contact != nullptr; contact = contact->NextSiblingElement())
        {

            QVariantMap contactInfo = getContactInfo(contact);
#ifdef DEBUG
            foreach(QString infoKey, contactInfo.keys())
            {

                printf("%s : %s\r\n"
                       ,infoKey.toStdString().c_str()
                       ,contactInfo[infoKey].value<QString>().toStdString().c_str()
                       );
            }
#endif
            if(contactInfo["PhoneNumbers"].toString() != "")
            {
                if(count != 0) printf("\u001b[0A\u001b[0A\r");
                count++;
                printf("Added: %d\r\n",count);
                printf("%s : %s\r\n",
                       contactInfo["FullName"].toString().toStdString().c_str(),
                       contactInfo["PhoneNumbers"].toString().toStdString().c_str());
//                sleep(1);
                std::string infoString =
                        contactInfo["FullName"].toString().toStdString()+","+
                        contactInfo["GivenName"].toString().toStdString()+","+
                        contactInfo["MiddleName"].toString().toStdString()+","+
                        contactInfo["FamilyName"].toString().toStdString()+","+
                        ",,,,,,,,,,,,,,,,,,,,,,,,* myContacts,,,,,Mobile,"+
                        contactInfo["PhoneNumbers"].toString().toStdString()+
                        +",,,,,,,,,,";
                FileController::addLine(outputFile,infoString);                
            }
        }
    }
    return 0;
}

QVariantMap getContactInfo(tinyxml2::XMLElement* contact)
{
    QVariantMap contactInfo;
    tinyxml2::XMLElement* key = contact->FirstChildElement();
    while(key != nullptr)
    {
        tinyxml2::XMLElement* value = key->NextSiblingElement();
        if(value != nullptr)
        {
#ifdef DEBUG
            printf("[%s:%s] (%s:%s)\r\n",
                   key->Name(),value->Name(),
                   key->GetText(),value->GetText());
#endif
            QString keyString(key->GetText());
            if(keyString.contains(QString("PhoneNumbers")))
            {
                QVariantMap phoneInfo = getContactNumber(value);
                contactInfo["PhoneNumbers"] = phoneInfo["Value"];
            }
            else
            {
                contactInfo[keyString] = QVariant::fromValue(QString(value->GetText()));
            }
        }
        key = value->NextSiblingElement();
    }
    return contactInfo;
}

QVariantMap getContactNumber(tinyxml2::XMLElement* contactSubItem)
{
    QVariantMap contactNumber;
    if(contactSubItem->FirstChildElement("dict") != nullptr)
    {
        tinyxml2::XMLElement* key = contactSubItem->FirstChildElement("dict")->FirstChildElement("key");
#ifdef DEBUG
        printf("    getContactNumber\r\n");
#endif
        while(key != nullptr)
        {
            tinyxml2::XMLElement* value = key->NextSiblingElement();

            if(value != nullptr)
            {
#ifdef DEBUG
                printf("    [%s:%s] (%s:%s)\r\n",
                       key->Name(),value->Name(),
                       key->GetText(),value->GetText());
#endif
                if(value != nullptr)
                {
                    contactNumber[QString(key->GetText())] = QVariant::fromValue(QString(value->GetText()));
                    key = value->NextSiblingElement();
                }
                else
                    break;
            }
            else break;
        }
    }
    return contactNumber;
}

void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}
