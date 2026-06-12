#include "repository.h"
#include "repositories/db_local.h"

Repository::Repository(QObject *parent, const QString &databaseName)
    : QObject(parent)
{
    m_db = new DB_LocalStorage(this, databaseName);
}

// emitted repository signals have listening slots in the model classes, and is how the models are being updated

// TODO: encryption/decryption, refactor database methods to reflect repository implementation

bool Repository::initDatabase() {
    return m_db->initDB();
}

// adds mock credential row to test fetching
void Repository::mockCredentialRow() {
    // group id, organization, username, pasword, email, additional optional notes
    bool mockAdded = m_db->addVaultRowEntry(
        1,
        "mock_organization",
        "mock_username",
        "mock_password",
        "mock_email",
        "mock_note"
    );
    if (mockAdded) {
        qDebug() << "[repository]: mock credential row was added to database";
        return;
    }

    qDebug() << "[repository]: error adding mock credential row to database";
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
void Repository::fetchGroups() {
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

    if (m_groupCache.isEmpty()) {
        emit groupCacheEmpty();
        return;
    }

    emit groupCacheUpdated(m_groupCache);
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
bool Repository::removeGroup(int index, int groupID) {
    if (m_db->removeGroup(groupID)) {
        emit groupEntryRemoved(index);
        return true;
    }

    return false;
}

// lambda effectively passes a template type Mapping object to the required paramater
void Repository::fetchCredentials(int groupID) {
    QList<Credential> m_credentialCache = m_db->fetchRecords<Credential>(
        "vault_content",
        {"org_name", "username", "password"},
        [](auto mapper) {
            Credential c;
            c.org_name = mapper("org_name").toString();
            c.username = mapper("username").toString();
            c.password = mapper("password").toString();
            c.email = mapper("email").toString();
            c.note = mapper("note").toString();
            return c;
        },
        "WHERE group_id = :groupID",
        {{":groupID", groupID}}
        );

    if (m_credentialCache.isEmpty()) {
        emit credentialCacheEmpty();
    }

    emit credentialCacheUpdated(m_credentialCache);
}

// called when previously filled credentials are left blank or removed
void Repository::removeCredential(Credential &credential) {

}

// called when an entire entry is removed
void Repository::removeCredentialRow(int contentID) {

}