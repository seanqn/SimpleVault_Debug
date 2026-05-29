#include "repository.h"
#include "repositories/db_local.h"
#include "credentialtypes.h"

Repository::Repository(DB_LocalStorage *_db, QObject *parent) {
    m_db = _db;
}

// TODO: encryption/decryption, refactor database methods to reflect repository implementation

void Repository::fetchCredentials(int groupID) {
    m_cache.clear();

    // QList<RawDbRow> rawRows = m_db->fetchCredentials(groupID);

    for (const auto& row : rawRows) {
        Credential c;
        c.id = row.id;
        c.username = row.username;
        // c.password = decrypt(row.encryptedPassword);

        m_cache.append(c);
    }

    emit dataChanged();
}

void Repository::addCredential(Credential credential) {
    // QString encryptedPasword = encrypt(credential.password);

    // int new_id = m_db->insertCredential(credential.username, encryptedPassword, credential.group_id);
    credential.id = new_id;

    m_cache.append(credential);

    emit entryAdded(m_cache.size() - 1);
}