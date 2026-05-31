#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H

#include <QObject>

class GroupsModel;
class CredentialModel;
class Repository;

// this controller class manages inbound write operations coming from QML to send insert/modify calls to the repository
// all read operations needed to update QML about the models can be called on from the public model methods
// all write operations needed to update the models themselves are found as public methods and will call on the repository and models respectively
class VaultManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(CredentialModel* CredentialModel READ CredentialModel CONSTANT)
    Q_PROPERTY(GroupsModel* GroupsModel READ GroupsModel CONSTANT)
    Q_PROPERTY(int getCurrentGroupID READ getCurrentGroupID)

public:
    explicit VaultManager(QObject* parent = nullptr, const QString &databaseName="SimpleVault");
    GroupsModel* GroupsModel() const { return m_groupsModel; }
    CredentialModel* CredentialModel() const { return m_credentialModel; }

    Q_INVOKABLE void createGroup(const QString &name);
    Q_INVOKABLE void selectGroup(int groupID);
    Q_INVOKABLE int getCurrentGroupID() const { return m_currentGroupID; }

signals:
    void groupAdded();
    void createGroupError();
    void groupChanged();
    void groupRenamed();
    void groupRemoved();
    void credentialToCache();
    void credentialRowToDatabase();


private:
    GroupsModel* m_groupsModel;
    CredentialModel* m_credentialModel;
    Repository* m_repository;
    int m_currentGroupID;
};

#endif // VAULTMANAGER_H
