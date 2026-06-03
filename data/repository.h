#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QObject>
#include "credentialtypes.h"

class DB_LocalStorage;

// Repository class communicates directly to the database, caching recent changes and handling encryption/decryption
class Repository : public QObject {
    Q_OBJECT
public:
    explicit Repository(QObject *parent = nullptr, const QString &databaseName="SimpleVault");

    int groupCount() const { return m_groupCache.size(); }
    int credentialRowCount() const { return m_credentialCache.size(); }
    // Group getGroupAt(int index) const { return m_groupCache.at(index); }
    // Credential getCredentialRowAt(int index) const { return m_credentialCache.at(index); }

    bool initDatabase();
    Group addGroup(const QString &groupName);
    QList<Group> fetchGroups();
    bool renameGroup(int groupID, const QString &newName);

    QList<Credential> fetchCredentials(int groupID);
    void addCredential(Credential &credential);
    void removeCredential(Credential &credential);
    void removeCredentialRow(int contentID);

signals:
    void cacheFilled();
    void credentialEntryAdded(int index);
    void credentialEntryRemoved(int index);

private:
    DB_LocalStorage *m_db;
    QList<Credential> m_credentialCache;
    QList<Group> m_groupCache;
};

#endif // REPOSITORY_H
