#include "repository.h"
#include "repositories/db_local.h"

Repository::Repository(QObject *parent, const QString &databaseName)
    : QObject(parent)
{
    m_db = new DB_LocalStorage(this, databaseName);
}

// TODO: encryption/decryption, refactor database methods to reflect repository implementation

bool Repository::initDatabase() {
    return m_db->initDB();
}

bool Repository::addGroup(const QString &groupName) {
    Group newGroup = m_db->addGroup(groupName);
    qDebug() << "[repository]: new group struct: [id]: " << newGroup.id << " [name]: " << newGroup.name << "[created_at]: " << newGroup.created_at;
    if (newGroup.id == 0) {
        return false;
    }

    emit groupEntryAdded(newGroup);
    return true;
}

// the only method where the cache is implemented is for the purpose of overwriting the group model's internal list
bool Repository::fetchGroups() {
    if (m_groupCache.isEmpty()) {
        return false;
    }

    m_groupCache = m_db->fetchRecords<Group>(
        "groups",
        {"id", "name", "created_at"},
        [](auto mapper) {
            Group g;
            g.id = mapper("id").toInt();
            g.name = mapper("name").toString();
            g.created_at = mapper("created_at").toDateTime();
            return g;
        }
    );

    emit groupCacheUpdated(m_groupCache);
    return true;
}

bool Repository::renameGroup(int groupID, const QString &newName) {
    if (!(m_db->renameGroup(groupID, newName))) {
        return false;
    }

    emit groupEntryRenamed(groupID, newName);
    return true;
}

// based on the foreign key groups id = vault_content group_id, removal of a group is expected to cascade to all associated credentials
// requires a return that is passed along to the controller to update the groups model
// also requires that the credential cache is updated to reflect the credentials removed in that group
bool Repository::removeGroup(int index, int groupID) {
    // the group id removes the group from the database, the passed index is corresponds to the group at the cache's index to remove
    if (m_db->removeGroup(groupID)) {
        emit groupEntryRemoved(index);
        return true;
    }

    return false;
}

// lambda effectively passes a template type Mapping object to the required paramater
// need to optimize this method for map cache
bool Repository::fetchCredentials(int groupID) {
    if (m_credentialCache.isEmpty()) {
        return false;
    }

    QList<Credential> m_credentialCache = m_db->fetchRecords<Credential>(
        "vault_content",
        {"group_id", "content_id", "org_name", "username", "password"},
        [](auto mapper) {
            Credential c;
            c.group_id = mapper("group_id").toInt();
            c.content_id = mapper("content_id").toInt();
            c.org_name = mapper("org_name").toString();
            c.username = mapper("username").toString();
            c.password = mapper("password").toString();
            return c;
        },
        "WHERE group_id = :groupID",
        {{":groupID", groupID}}
        );

    emit credentialCacheUpdated(m_credentialCache);
    return true;
}

// called when previously filled credentials are left blank or removed
void Repository::removeCredential(Credential &credential) {

}

// called when an entire entry is removed
void Repository::removeCredentialRow(int contentID) {

}