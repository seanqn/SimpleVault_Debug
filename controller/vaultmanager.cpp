#include "vaultmanager.h"
#include "data/repository.h"

// TODO: CredentialModel and Repository incomplete

// TODO: QSortFilterProxyModel can filter vault content based on group_id via the groups model without needing to restructure how the data is retrieved from the repository

// REFACTOR: removing read operations away from controller/repoository communication and just letting the repository handle model read updates via signals
VaultManager::VaultManager(QObject *parent, const QString &databaseName)
    : QObject(parent) {

    m_repository = new Repository(this, databaseName);

    m_groupsModel = new GroupsModel(this);
    m_credentialModel = new CredentialModel(this);

    initRepository();

    connect(m_repository, &Repository::groupEntryAdded, m_groupsModel, &GroupsModel::append);
    connect(m_repository, &Repository::groupEntryRemoved, m_groupsModel, &GroupsModel::remove);
    connect(m_repository, &Repository::groupEntryRenamed, m_groupsModel, &GroupsModel::rename);

    connect(this, &VaultManager::credentialRowAdded, m_credentialModel, &CredentialModel::appendRow);
    // connect(m_repository, &Repository::credentialRowAdded, m_credentialModel, &CredentialModel::appendRow);
    // connect(m_repository, &Repository::credentialRowRemoved, m_credentialModel, &CredentialModel::removeRow);
    // connect(m_repository, &Repository::credentialColumnModified, m_credentialModel, &CredentialModel::modifyColumn);

    updateGroups();
}

void VaultManager::initRepository() {
    if (!m_repository->initDatabase()) emit repositoryInitializationError();
}

// ideally should only be called by the constructor and other members
void VaultManager::updateGroups() {
    if (m_repository->fetchGroups()) {
        return;
    }

    emit groupsUpdateError();
}

// database write entry is added to database and only appended to the cache
void VaultManager::createGroup(const QString &name) {
    if (m_repository->addGroup(name)) {
        emit groupAdded();
        return;
    }

    emit createGroupError();
}

void VaultManager::selectGroup(int groupID) {
    m_currentGroupID = groupID;
    emit groupChanged();
    if (m_repository->fetchCredentials(groupID)) {
        return;
    }
}

// rename will be selected via a right-click context menu option, which is not necessarily reflective of the current (double-click selected) group id
// QML will know what group is actually in context since it will be assigned as the current index, and then gets the group id at that index so that it is updated in the database
void VaultManager::renameGroup(int groupID, const QString &newName) {
    if (m_repository->renameGroup(groupID, newName)) {
        emit groupRenamed(newName);
        return;
    }

    emit groupRenameError();
}

void VaultManager::removeGroup(int index, int groupID) {
    if (m_repository->removeGroup(index, groupID)) {
        emit groupRemoved(groupID);
        m_currentGroupID = 0;
        emit groupChanged();
        return;
    }

    emit groupRemoveError(groupID);
}

// credential model management methods
void VaultManager::addCredentialRow() {
    emit credentialRowAdded();
}