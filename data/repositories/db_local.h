#ifndef DB_LOCAL_H
#define DB_LOCAL_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

struct Credential;

// QML invokable macro will likely be redundant after repository is complete

class DB_LocalStorage : public QObject {
    Q_PROPERTY(bool isDBConnected READ isDBConnected NOTIFY databaseConnectionChange)
public:
    explicit DB_LocalStorage(QObject *parent = nullptr, const QString &databaseName="SimpleVault");
    ~DB_LocalStorage();

    bool initDB();
    void closeDB();
    int addGroup(const QString &groupName);
    QString fetchGroupName(int groupID);
    bool renameGroup(int groupID, const QString &newGroupName);
    bool removeGroup(int groupID);
    bool addVaultRowEntry(int currGroupID, const QString &organizationName, const QString &username, const QString &pass);

    template <typename T, typename Mapping>
    QList<Credential> fetchCredentials(int currGroupID, Mapping mapper);
    // Q_INVOKABLE void removeVaultContent();
    bool isDBConnected();

signals:
    void initDBFailure(const QSqlError& error);
    void startDBConnectionFailure(const QSqlError &error);
    void closeDBConnectionFailure(const QSqlError &error);
    void startDBConnectionSuccess();
    void closeDBConnectionSuccess();
    void directoryEstablished(QString &dir);
    void databasePathEstablished(QString &path);
    void databaseQueryError(const QSqlError &error);
    void databaseQuerySuccess();
    void databaseConnectionChange();

private:
    QSqlDatabase _db;
    bool m_connected;
    QString m_databaseName;
};

// provides a collection of all of the credentials for a selected group based on the provided mapping function
// the mapping function will be expected to map each column value to its respective member as defined in the Credential struct
template <typename T, typename Mapping>
QList<Credential> DB_LocalStorage::fetchCredentials(int currGroupID, Mapping mapper) {
    QList<Credential> credentials;
    QSqlQuery _query(_db);
    _query.prepare("SELECT content_id, org_name, username, password FROM vault_content WHERE group_id = :currGroupID");
    _query.bindValue(":currGroupID", currGroupID);

    if (!_query.exec()) {
        emit databaseQueryError(_query.lastError());
        return credentials;
    }

    QSqlRecord record = _query.record();

    auto rowMapper = mapper(record);

    while (_query.next()) {
        credentials.append(rowMapper(_query));
    }

    return credentials;
}

#endif // DB_LOCAL_H
