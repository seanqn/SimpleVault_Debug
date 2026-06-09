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
    Q_PROPERTY(int getCurrentContentID READ getCurrentContentID NOTIFY credentialRowChanged)
    // Q_PROPERTY(int getCredentialRowCount READ getCredentialRowCount)


public:
    explicit VaultManager(QObject* parent = nullptr, const QString &databaseName="SimpleVault");
    GroupsModel* groupsModel() const { return m_groupsModel; }
    CredentialModel* credentialModel() const { return m_credentialModel; }
    // int getCredentialRowCount() { return m_credentialModel->rowCount(); }
    void initRepository();
    void updateGroups();
    void updateCredentials();

    Q_INVOKABLE void createGroup(const QString &name);
    Q_INVOKABLE void selectGroup(int groupID);
    Q_INVOKABLE void renameGroup(int groupID, const QString &newName);
    Q_INVOKABLE void removeGroup(int index, int groupID);
    Q_INVOKABLE int getCurrentGroupID() const { return m_currentGroupID; }

    // Q_INVOKABLE void createCredentialRow();

    /* modify includes removing, renaming, or assigning a credential to a credential row
    (technically reassigned to the corresponding Credential struct member, equivalent to adding a new column value row)
    */
    Q_INVOKABLE void addCredentialRow();
    // Q_INVOKABLE void modifyCredential();
    // Q_INVOKABLE void removeCredentialRow();
    Q_INVOKABLE int getCurrentContentID() const { return m_currentContentID; }

signals:
    void repositoryInitializationError();

    void groupAdded();
    void createGroupError();
    void groupsUpdateError();
    void groupChanged();
    void groupRenamed(const QString &name);
    void groupRenameError();
    void groupRemoved(int id);
    void groupRemoveError(int id);

    // void credentialAdded();
    // void createCredentialsError();
    // void credentialsUpdateError();
    void credentialRowAdded();
    void credentialRowChanged();
    // void credentialModified();
    // void credentialRowRemoveError();

private:
    GroupsModel* m_groupsModel;
    CredentialModel* m_credentialModel;
    Repository* m_repository;
    int m_currentGroupID;
    int m_currentContentID;
};

#endif // VAULTMANAGER_H
