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

    // these getters are redundant since the cache is only reflective of the tables at build and not for live modifications
    // Group getGroupAt(int index) const { return m_groupCache[index]; }
    // Credential getCredentialRowAt(int index) const { return m_credentialCache.value(index); }

    bool initDatabase();
    bool addGroup(const QString &groupName);
    bool fetchGroups();
    bool renameGroup(int groupID, const QString &newName);
    bool removeGroup(int index, int groupID);

    bool fetchCredentials(int groupID);
    bool addCredential(int groupID);
    void removeCredential(Credential &credential);
    void removeCredentialRow(int contentID);

signals:
    void groupEntryAdded(Group &group);
    void groupEntryRemoved(int index);
    void groupEntryRenamed(int index, const QString &name);
    void groupCacheUpdated(QList<Group> &cache);
    void credentialCacheUpdated(QList<Credential> &cache);
    void credentialRowAdded();
    void credentialRowRemoved(int index);

private:
    DB_LocalStorage *m_db;
    QList<Credential> m_credentialCache;
    QList<Group> m_groupCache;
};

#endif // REPOSITORY_H
