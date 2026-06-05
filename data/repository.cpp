#include "repository.h"
#include "repositories/db_local.h"

Repository::Repository(QObject *parent, const QString &databaseName)
    : QObject(parent)
{
    m_db = new DB_LocalStorage(this, databaseName);

    m_groupCache.clear();
    m_credentialCache.clear();
}

// TODO: implement fetchGroup() as a function or extend logic in addGroup() to provide VaultManager a way to update the GroupsModel with a mapped Group struct
// TODO: a new cache will have to be implemented as a private member to store all created groups (this may be able to leverage the group_id column in CredentialsModel)
// TODO: encryption/decryption, refactor database methods to reflect repository implementation
bool Repository::initDatabase() {
    return m_db->initDB();
}

// it is likely better to just call fetchGroups to return the cache back to the controller and along to the model
// keeping this logic now to test funcion
Group Repository::addGroup(const QString &groupName) {
    Group newGroup = m_db->addGroup(groupName);
    qDebug() << "[repository]: new group struct: [id]: " << newGroup.id << " [name]: " << newGroup.name << "[created_at]: " << newGroup.created_at;
    m_groupCache.append(newGroup);
    qDebug() << "groupCache size: " << m_groupCache.size();
    return newGroup;
}

QList<Group> Repository::fetchGroups() {
    if (m_groupCache.isEmpty()) {
        return m_groupCache;
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

    emit cacheFilled();
    return m_groupCache;
}

Group Repository::renameGroup(Group &group, const QString &newName) {
    if (m_db->renameGroup(group.id, newName)) {
        group.name = newName;
    }

    return group;
}

// based on the foreign key groups id = vault_content group_id, removal of a group is expected to cascade to all associated credentials
// requires a return that is passed along to the controller to update the groups model
// also requires that the credential cache is updated to reflect the credentials removed in that group
QList<Group> Repository::removeGroup(int index, int groupID) {
    // the group id removes the group from the database, the passed index is corresponds to the group at the cache's index to remove
    m_db->removeGroup(groupID);
    if (m_groupCache.remove(index) == 1) {
        return true;
    }
    return false;
    // call to remove credentials
}

// lambda effectively passes a template type Mapping object to the required paramater
// need to optimize this method for map cache
// QList<Credential> Repository::fetchCredentials(int groupID) {
//     if (m_credentialCache.isEmpty()) {
//         QList<Credential> recordList = m_db->fetchRecords<Credential>(
//             "vault_content",
//             {"group_id", "content_id", "org_name", "username", "password"},
//             [](auto mapper) {
//                 Credential c;
//                 c.group_id = mapper("group_id").toInt();
//                 c.content_id = mapper("content_id").toInt();
//                 c.org_name = mapper("org_name").toString();
//                 c.username = mapper("username").toString();
//                 c.password = mapper("password").toString();
//                 return c;
//             }
//             );

//         emit cacheFilled();
//         return m_credentialCache;
//     }

//     for (const auto &c : recordList) {
//         if (c.group_id == groupID) {
//             m_credentialCache.insert(c);
//         }
//     }

//     return filteredCredentials;
// }

void Repository::addCredential(Credential &credential) {
    // QString encryptedPasword = encrypt(credential.password);

    // int new_id = m_db->insertCredential(credential.username, encryptedPassword, credential.group_id);
    // credential.content_id = new_id;

    m_credentialCache.insert(credential);

    emit credentialEntryAdded(m_credentialCache.size() - 1);
}

// called when previously filled credentials are left blank or removed
void Repository::removeCredential(Credential &credential) {

}

// called when an entire entry is removed
void Repository::removeCredentialRow(int contentID) {

}