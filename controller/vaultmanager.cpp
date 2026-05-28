#include "vaultmanager.h"
#include "data/repositories/db_local.h"
#include "data/repository.h"

VaultManager::VaultManager(QObject* parent, const QString &connectionName)
    : QObject(parent)
{
    m_db = new DB_LocalStorage(this, connectionName);
    QObject::connect(m_db, &DB_LocalStorage::directoryEstablished, [](QString path){
        qDebug() << "DB Path established at: " << path;
    });
    if (m_db->initDB()) {
        m_groupsModel = new GroupsModel(this, connectionName);
        m_vaultContentModel = new VaultContentModel(this, connectionName);
    }
    else {
        // signal
    }
}

void VaultManager::setCurrentGroup(int currentGroupID) {
    m_currentGroupID = currentGroupID;
    m_repository->fetchCredentials(currentGroupID);

    emit currentGroupIDChanged(currentGroupID);
}

void VaultManager::addGroup(const QString &groupName) {
    int _id = m_db->addGroup(groupName);
    if (_id == -1) {
        return;
    }
    // m_repository->addGroup(groupName);

    emit addGroupSelectAccepted(_id);
}

// this method implies that a group can be removed without having active focus on it in the pane
// i.e. removing via context menu right-click or bulk selection
void VaultManager::removeGroup(int groupID) {
    if (!m_db->removeGroup(groupID)) {
        return;
    }
    m_groupsModel->select();

    if (groupID == m_currentGroupID) {
        m_vaultContentModel->setFilter("1=0");
        m_vaultContentModel->select();
        m_currentGroupID = -1;
        emit currentGroupIDChanged(groupID);
    }

    emit removeGroupSelectAccepted(groupID);
}

void VaultManager::renameGroup(int groupID, const QString &newName) {
    if (!m_db->renameGroup(groupID, newName)) {
        return;
    }
    m_groupsModel->select();

    emit groupNameChanged(m_db->fetchGroupName(groupID));
}

void VaultManager::addVaultRowEntry(int groupID, const QString &organizationName, const QString &username, const QString &password) {
    if (groupID <= 0){
        return;
    }
    if (!m_db->addVaultContent(groupID, organizationName, username, password)) {
        return;
    }

    if (groupID == m_currentGroupID) {
        m_vaultContentModel->select();
    }
    emit vaultRowEntrySuccess();
}
