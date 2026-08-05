#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H

#include <QObject>
#include <QUuid>
#include <QDateTime>
// MOC requires complete types in the header for classes in a Q_PROPERTY so no forward declaration for them
#include "models/credentialmodel.h"
#include "models/groupsmodel.h"

class Repository;

// UPDATE: many of these methods will be implemented as service classes after prototyping/debugging the data layer is complete
class VaultManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(CredentialModel* credentialModel READ credentialModel CONSTANT)
    Q_PROPERTY(GroupsModel* groupsModel READ groupsModel CONSTANT)

    Q_PROPERTY(bool isGroupSelected READ isGroupSelected NOTIFY groupChanged)
    Q_PROPERTY(QString getCurrentGroupID READ getCurrentGroupID NOTIFY groupChanged)
    Q_PROPERTY(QString getCurrentContentID READ getCurrentContentID)
    // Q_PROPERTY(int getCredentialRowCount READ getCredentialRowCount)

public:
    explicit VaultManager(QObject* parent = nullptr, const QString &databaseName="SimpleVault");
    ~VaultManager();
    GroupsModel* groupsModel() const { return m_groupsModel; }
    CredentialModel* credentialModel() const { return m_credentialModel; }
    void initRepository();
    // void updateGroups();

    // group services (GroupsModel)
    Q_INVOKABLE void createGroup(const QString &name);
    Q_INVOKABLE void selectGroup(int index);
    Q_INVOKABLE void renameGroup(int index, const QString &newName);
    Q_INVOKABLE void removeGroup(int index);
    bool isGroupSelected() { return !m_currentGroupID.isNull(); }
    Q_INVOKABLE QString getCurrentGroupID() const { return QUuid::fromRfc4122(m_currentGroupID).toString(); }

    // credential content services (CredentialModel)
    Q_INVOKABLE void addDefaultCredentialRow();
    Q_INVOKABLE void selectCredentialRow(int rowIndex);
    // Q_INVOKABLE void removeCredentialRow();
    Q_INVOKABLE QString getCurrentContentID() const { return QUuid::fromRfc4122(m_currentContentID).toString(); }

    // edit cache service
    Q_INVOKABLE void startRowEdit(int rowIndex);
    Q_INVOKABLE int getEditRowIndex() const { return m_editRowIndex; }
    Q_INVOKABLE void updateEditCache(const QString &role, const QString &value);
    Q_INVOKABLE void submitAndResetEditCache();
    Q_INVOKABLE bool editCacheIsEmpty();
    Q_INVOKABLE bool editCacheIsClean();

signals:
    void repositoryInitializationError();
    void groupAdded();
    void createGroupError();
    void groupChanged(const QString &id);
    void groupRenamed(const QString &name);
    void groupRenameError();
    void groupRemoved(const QString &id);
    void groupRemoveError(const QString &id);

private:
    GroupsModel* m_groupsModel;
    CredentialModel* m_credentialModel;
    Repository* m_repository;

    QByteArray m_currentGroupID;
    QByteArray m_currentContentID;

    // edit cache members
    bool defaultRowNotSubmitted = false;
    Credential m_row; // stores rows that already exist in the model
    Credential m_editCache; // stores currently edited row (at edit row index) and serves as a comparater to determine if the row actually need be submitted
    int m_editRowIndex = -1;
};

#endif // VAULTMANAGER_H
