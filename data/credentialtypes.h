#ifndef CREDENTIALTYPES_H
#define CREDENTIALTYPES_H
#include <QDateTime>
// #include <QObject>
// #include <QQmlEngine>

// outlines the schema for groups and credentials in database
struct Group {
public:
    int id = 0;
    QString name;
    QDateTime created_at;
};

struct Credential {
public:
    // now storing content ids as (credentialtype: QByteArray=QUuid, database: BLOB) for safer id creation and keying
    // creation is at VaultManager::addDefaultCredentialRow
    QByteArray content_id;
    QString org_name;
    QString username;
    QString password;
    QString email;
    QString note;
};

// Q_DECLARE_METATYPE(Credential)

#endif // CREDENTIALTYPES_H
