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

    connect(m_repository, &Repository::groupCacheEmpty, this, &VaultManager::repositoryGroupCacheEmpty);
    connect(m_repository, &Repository::credentialCacheEmpty, this, &VaultManager::repositoryCredentialCacheEmpty);

    connect(m_repository, &Repository::groupCacheUpdated, m_groupsModel, &GroupsModel::update);
    connect(m_repository, &Repository::groupEntryAdded, m_groupsModel, &GroupsModel::append);
    connect(m_repository, &Repository::groupEntryRemoved, m_groupsModel, &GroupsModel::remove);
    connect(m_repository, &Repository::groupEntryRenamed, m_groupsModel, &GroupsModel::rename);

    connect(m_repository, &Repository::credentialCacheUpdated, m_credentialModel, &CredentialModel::update);
    connect(this, &VaultManager::credentialRowAdded, m_credentialModel, &CredentialModel::appendRow);
    connect(m_repository, &Repository::credentialRowUpsert, m_credentialModel, &CredentialModel::upsert);
    // connect(m_repository, &Repository::credentialRowAdded, m_credentialModel, &CredentialModel::appendRow);
    // connect(m_repository, &Repository::credentialRowRemoved, m_credentialModel, &CredentialModel::removeRow);
    // connect(m_repository, &Repository::credentialColumnModified, m_credentialModel, &CredentialModel::modifyColumn);

    updateGroups();
    m_repository->mockCredentialRow();
}

void VaultManager::initRepository() {
    if (!m_repository->initDatabase()) emit repositoryInitializationError();
}

// groups model management methods

// ideally should only be called by the constructor and other members
void VaultManager::updateGroups() {
    m_repository->fetchGroups();
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
    // seems to be some discrepancy between actual group id in the db table and autoincremented model IDRole values
    m_repository->fetchCredentials(groupID);
    m_currentGroupID = groupID;
    emit groupChanged(m_currentGroupID);
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
        emit groupChanged(m_currentGroupID);
        return;
    }

    emit groupRemoveError(groupID);
}

// credential model management methods
void VaultManager::addCredentialRow() {
    emit credentialRowAdded(Credential());
}

void VaultManager::upsertCredentialRow(int groupID, Credential &credential) {
    // call repository method
    m_repository->upsertCredentialRow(groupID, credential);
}

// slot functions
void VaultManager::repositoryGroupCacheEmpty() {
    emit groupsUpdated("Groups cache empty");
}

void VaultManager::repositoryCredentialCacheEmpty() {
    emit credentialsUpdated("Credential cache empty");
}