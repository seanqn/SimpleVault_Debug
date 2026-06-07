#ifndef DB_LOCAL_H
#define DB_LOCAL_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlDriver>
#include "data/credentialtypes.h"

class DB_LocalStorage : public QObject {
    Q_OBJECT
    // Q_PROPERTY(bool isDBConnected READ isDBConnected NOTIFY databaseConnectionChange)

public:
    explicit DB_LocalStorage(QObject *parent = nullptr, const QString &databaseName="SimpleVault");
    ~DB_LocalStorage();

    bool initDB();
    void closeDB();
    Group addGroup(const QString &groupName);
    QString fetchGroupName(int groupID);
    bool renameGroup(int groupID, const QString &newGroupName);
    bool removeGroup(int groupID);
    bool addVaultRowEntry(int currGroupID, const QString &organizationName, const QString &username, const QString &pass);
    bool isDBConnected();

    // read operations specifically called by VaultManager to update the models
    template <typename T, typename Mapping>
    QList<T> fetchRecords(const QString &table, const QStringList &columns, Mapping mapper);

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

// method serves as a general utility method which can fetch from any table based on column names
// the mapper parameter will expect function calls to consolidate passed in parameters in lambda form

/*
updated logic includes an argument that accepts a conditional statement which is important for fetching
credentials based on the current group id
another argument accepts bindings for condition variables
*/
template <typename T, typename Mapping>
QList<T> DB_LocalStorage::fetchRecords(
    const QString &table,
    const QStringList &columns,
    Mapping mapper,
    const QString &condition = QString(),
    const QVariantMap &bindings = QVariantMap()
    ) {

    QList<T> values;

    // collect the request and append the condition if necessary
    QString request = QString("SELECT %1 FROM %2").arg(columns.join(", "), table);
    if (!condition.isEmpty()) {
        request += " " + condition;
    }

    QSqlQuery _query(_db);
    if (!_query.prepare(request)) {
        emit databaseQueryError(_query.lastError());
        return values;
    }

    // iterate through the map if it was passed and bind variables
    if (!bindings.isEmpty()) {
        QVariantMap::const_iterator i = bindings.constBegin();
        while (i != bindings.constEnd()) {
            _query.bindValue(i.key(), i.value());
            ++i;
        }
    }

    if (!_query.exec()) {
        emit databaseQueryError(_query.lastError());
        return values;
    }

    // start fetching the fields
    QSqlRecord record = _query.record();

    // map logic
    QHash<QString, int> columnToIndex;
    columnToIndex.reserve(columns.size());
    for (const QString &col : columns) {
        columnToIndex.insert(col, record.indexOf(col));
    }

    if (_query.driver()->hasFeature(QSqlDriver::QuerySize)) {
        values.reserve(_query.size());
    }

    while (_query.next()) {
        auto valueName = [&](const QString &colName) -> QVariant {
            int index = columnToIndex.value(colName, -1);
            return (index != -1) ? _query.value(index) : QVariant();
        };

        values.append(mapper(valueName));
    }

    return values;
}

// this needs to be tested
// saved method for potential to correspond logic with fetchCredentisl() logic

// template <typename Mapping>
// Group DB_LocalStorage::fetchGroups(Mapping mapper) {
//     Group group;
//     QSqlQuery _query(_db);
//     _query.prepare("SELECT id, name, created_at FROM groups WHERE id = :currGroupID");
//     _query.bindValue(":currGroupID", currGroupID);

//     if (!_query.exec()) {
//         emit databaseQueryError(_query.lastError());
//         return group;
//     }

//     QSqlRecord = _query.record;

//     auto rowMapper = mapper(record);
//     while (_query.next()) {
//         group.append(rowMapper(_query));
//     }

//     return group;
// }

#endif // DB_LOCAL_H
