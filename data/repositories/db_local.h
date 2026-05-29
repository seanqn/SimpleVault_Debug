#ifndef DB_LOCAL_H
#define DB_LOCAL_H
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

// QML invokable macro will likely be redundant after repository is complete

class DB_LocalStorage : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isDBConnected READ isDBConnected NOTIFY isDBConnectedChanged)
public:
    explicit DB_LocalStorage(QObject *parent = nullptr, const QString &databaseName="SimpleVault");
    ~DB_LocalStorage();

    Q_INVOKABLE bool initDB();
    Q_INVOKABLE void closeDB();
    // Q_INVOKABLE void createTables();
    Q_INVOKABLE int addGroup(const QString &groupName);
    Q_INVOKABLE QString fetchGroupName(int groupID);
    Q_INVOKABLE bool renameGroup(int groupID, const QString &newGroupName);
    Q_INVOKABLE bool removeGroup(int groupID);

    Q_INVOKABLE bool addVaultRowEntry(int currGroupID, const QString &organizationName, const QString &username, const QString &pass);
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
    void isDBConnectedChanged();

private:
    QSqlDatabase _db;
    bool m_connected;
    QString m_databaseName;

};

#endif // DB_LOCAL_H
