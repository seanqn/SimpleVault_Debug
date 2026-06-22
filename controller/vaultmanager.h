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
    void initRepository();
    void updateGroups();

    Q_INVOKABLE void createGroup(const QString &name);
    Q_INVOKABLE void selectGroup(int groupID);
    Q_INVOKABLE void renameGroup(int groupID, const QString &newName);
    Q_INVOKABLE void removeGroup(int index, int groupID);
    Q_INVOKABLE int getCurrentGroupID() const { return m_currentGroupID; }

    // adds empty row, if the row columns contain any default values after editing, the row is deconstructed
    Q_INVOKABLE void addDefaultCredentialRow();
    Q_INVOKABLE void selectCredentialRow(int rowIndex);
    // Q_INVOKABLE void removeCredentialRow();
    Q_INVOKABLE int getCurrentContentID() const { return m_currentContentID; }
    Q_INVOKABLE void startRowEdit(int rowIndex);
    Q_INVOKABLE void updateRowCacheField(const QString &role, const QString &value);
    Q_INVOKABLE void submitRow();

signals:
    void repositoryInitializationError();

    void groupAdded();
    void createGroupError();
    void groupsUpdated(const QString &msg);
    void groupChanged(int id);
    void groupRenamed(const QString &name);
    void groupRenameError();
    void groupRemoved(int id);
    void groupRemoveError(int id);

    // void createCredentialsError();
    void credentialsUpdated(const QString &msg);
    // argument is expected to be default, but matches that of the listening slot
    void credentialRowAdded(const Credential = Credential());
    void credentialRowChanged(int id);
    void credentialRowUpdated(const Credential &credential);
    // void credentialRowRemoved();
    // void credentialRowRemoveError();

public slots:
    void repositoryGroupCacheEmpty() { emit groupsUpdated("Group cache empty"); }
    void repositoryCredentialCacheEmpty() { emit credentialsUpdated("Credential cache empty"); }

private:
    GroupsModel* m_groupsModel;
    CredentialModel* m_credentialModel;
    Repository* m_repository;
    int m_currentGroupID;
    int m_currentContentID;
    Credential m_rowCache;
    int m_editRowIndex = -1;
};

#endif // VAULTMANAGER_H
