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
    int groupID = 1;
    Credential mockRow;
    mockRow.content_id = QUuid::createUuidV7().toRfc4122();
    qDebug() << "[repository]: calling mockCredentialRow for group " << groupID << ", content id: " << QUuid::fromRfc4122(mockRow.content_id).toString();
    mockRow.org_name = "mock_org";
    mockRow.username = "mock_user";
    mockRow.password = "mock_pass";
    mockRow.email = "mock_email";
    mockRow.note = "mock_note";
    upsertCredentialRow(groupID, mockRow);
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
    qDebug() << "[repository]: fetching groups";
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
        qDebug() << "[repository]: group cache was empty";
        return;
    }

    emit groupCacheUpdated(m_groupCache);
    qDebug() << "[repository]: group cache was updated with the items from database";
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
    qDebug() << "[repository]: fetching credentials";
    QList<Credential> m_credentialCache = m_db->fetchRecords<Credential>(
        "vault_content",
        {"content_id", "org_name", "username", "password", "email", "note"},
        [](auto mapper) {
            Credential c;
            c.content_id = mapper("content_id").toByteArray();
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
        qDebug() << "[repository]: credential cache was empty";
    }

    emit credentialCacheUpdated(m_credentialCache);
    qDebug() << "[repository]: credential cache was updated with items from database";
}

void Repository::upsertCredentialRow(int groupID, Credential &credential) {
    bool upsertedRowContentID = m_db->upsertVaultRowEntry(groupID, credential);

    if (!upsertedRowContentID) {
        qDebug() << "[repository]: credential row could not be upserted.";
        return;
    }

    qDebug() << "[repository]: credential row successfully upserted";
}

// called when an entire entry is removed
void Repository::removeCredentialRow(int groupID, int contentID) {

}