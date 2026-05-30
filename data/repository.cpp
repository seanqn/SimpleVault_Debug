#include "repository.h"
#include "repositories/db_local.h"
#include "credentialtypes.h"

Repository::Repository(DB_LocalStorage *_db, QObject *parent) {
    m_db = _db;
}

// TODO: encryption/decryption, refactor database methods to reflect repository implementation

QList<Credential> Repository::fetchCredentials(int groupID) {
    m_cache.clear();
    m_cache = m_db->fetchCredentials<Credential>(groupID, [](const QSqlRecord &record) {
        // retrieve column indices
        const int contentIndex = record.indexOf("content_id");
        const int orgIndex = record.indexOf("org_name");
        const int userIndex = record.indexOf("username");
        const int passIndex = record.indexOf("password");

        // map column values to respective Credential struct members to cache all entries for this current group
        return [=](const QSqlQuery &query) {
            Credential c;
            c.group_id = groupID;
            c.content_id = query.value(contentIndex).toInt();
            c.org_name = query.value(orgIndex).toString();
            c.username = query.value(userIndex).toString();
            c.password = query.value(passIndex).toString();
            return c;
        };
    });

    emit cacheFilled();
    return m_cache;
}

void Repository::addCredential(Credential &credential) {
    // QString encryptedPasword = encrypt(credential.password);

    // int new_id = m_db->insertCredential(credential.username, encryptedPassword, credential.group_id);
    credential.content_id = new_id;

    m_cache.append(credential);

    emit entryAdded(m_cache.size() - 1);
}

// called when previously filled credentials are left blank or removed
void Repository::removeCredential(Credential &credential) {

}

// called when an entire entry is removed
void Repository::removeCredentialRow(int contentID) {

}