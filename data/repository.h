#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QObject>
#include "credentialtypes.h"

class DB_LocalStorage;
struct Credential;

// Repository class communicates directly to the database, caching recent changes and handling encryption/decryption
class Repository : public QObject {
    Q_OBJECT
public:
    explicit Repository(DB_LocalStorage *_db, QObject *parent = nullptr);

    int count() const { return m_cache.size(); }
    Credential getAt(int index) const { return m_cache.at(index); }

    void fetchCredentials(int groupID);
    void addCredential(Credential credential);
    void removeCredential(int id);

signals:
    void dataChanged();
    void entryAdded(int index);
    void entryRemoved(int index);

private:
    DB_LocalStorage *m_db;
    QList<Credential> m_cache;
};

#endif // REPOSITORY_H
