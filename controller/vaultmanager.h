#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H

#include <QObject>
// MOC requires complete types in the header for classes in a Q_PROPERTY so no forward declaration for them
#include "models/credentialmodel.h"
#include "models/groupsmodel.h"

class Repository;

// this controller class manages inbound write operations coming from QML to send insert/modify calls to the repository
// all read operations needed to update QML about the models can be called on from the public QML model properties here
// all write operations needed to update the models themselves are found as public methods and will call on the repository and models respectively
class VaultManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(CredentialModel* credentialModel READ credentialModel CONSTANT)
    Q_PROPERTY(GroupsModel* groupsModel READ groupsModel CONSTANT)
    Q_PROPERTY(int getCurrentGroupID READ getCurrentGroupID NOTIFY groupChanged)

public:
    explicit VaultManager(QObject* parent = nullptr, Repository* repository, const QString &databaseName="SimpleVault");
    GroupsModel* groupsModel() const { return m_groupsModel; }
    CredentialModel* credentialModel() const { return m_credentialModel; }

    void initRepository();
    void updateGroups();
    void updateCredentials();
    Q_INVOKABLE void createGroup(const QString &name);
    Q_INVOKABLE void selectGroup(int groupID);
    Q_INVOKABLE void renameGroup(int index, const QString &newName);
    Q_INVOKABLE void removeGroup(int index, int groupID);
    Q_INVOKABLE int getCurrentGroupID() const { return m_currentGroupID; }

signals:
    void repositoryInitializationError();
    void groupAdded();
    void createGroupError();
    void groupsUpdateError();
    void credentialsUpdateError();
    void groupChanged();
    void groupRenamed(int id, QString name);
    void groupRenameError(int id, QString name);
    void groupRemoved(int id);
    void credentialToCache();
    void credentialRowToDatabase();


private:
    GroupsModel* m_groupsModel;
    CredentialModel* m_credentialModel;
    Repository* m_repository;
    int m_currentGroupID;
};

#endif // VAULTMANAGER_H
