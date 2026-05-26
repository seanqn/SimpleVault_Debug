#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H
#include "data/repositories/db_local.h"
#include "models/groupsmodel.h"
#include "models/vaultcontentmodel.h"

class VaultManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(GroupsModel* groupsModel READ groupsModel NOTIFY groupsModelChanged)
    Q_PROPERTY(int currentGroupID READ currentGroupID NOTIFY currentGroupIDChanged)

public:
    explicit VaultManager(QObject *parent = nullptr, const QString &connectionName="SimpleVault");
    Q_INVOKABLE GroupsModel* groupsModel() const { return m_groupsModel; }
    Q_INVOKABLE int currentGroupID() const { return m_currentGroupID; }
    Q_INVOKABLE void setCurrentGroup(int currentGroupID);
    Q_INVOKABLE void addGroup(const QString &groupName);
    Q_INVOKABLE void removeGroup(int groupID);
    Q_INVOKABLE void renameGroup(int groupID, const QString &newName);
    Q_INVOKABLE void addVaultRowEntry(int groupID, const QString &organizationName, const QString &username, const QString &password);

signals:
    void currentGroupIDChanged(int groupID);
    void groupsModelChanged();
    void addGroupSelectAccepted(int groupID);
    void groupNameChanged(const QString &groupName);
    void removeGroupSelectAccepted(int groupID);
    void vaultRowEntrySuccess();

private:
    DB_LocalStorage* m_db;
    GroupsModel* m_groupsModel;
    VaultContentModel* m_vaultContentModel;
    int m_currentGroupID;
};

#endif // VAULTMANAGER_H
