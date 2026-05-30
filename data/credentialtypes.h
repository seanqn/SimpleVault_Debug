#ifndef CREDENTIALTYPES_H
#define CREDENTIALTYPES_H

#include <QObject>
#include <QQmlEngine>

// Credential struct, exposed to QML, outlines the schema for credentials in database
struct Credential {
    Q_GADGET
    QML_VALUE_TYPE(credential)

    Q_PROPERTY(int group_id MEMBER group_id)
    Q_PROPERTY(int content_id MEMBER content_id)
    Q_PROPERTY(QString org_name MEMBER org_name)
    Q_PROPERTY(QString username MEMBER username)
    Q_PROPERTY(QString password MEMBER password)

public:
    int group_id = 0;
    int content_id = 0;
    QString org_name;
    QString username;
    QString password;
};

Q_DECLARE_METATYPE(Credential)

#endif // CREDENTIALTYPES_H
