#ifndef CREDENTIALTYPES_H
#define CREDENTIALTYPES_H

#include <QObject>
#include <QQmlEngine>

// outlines the schema for credentials in database
struct Credential {
public:
    int group_id = 0;
    int content_id = 0;
    QString org_name;
    QString username;
    QString password;
};

Q_DECLARE_METATYPE(Credential)

#endif // CREDENTIALTYPES_H
