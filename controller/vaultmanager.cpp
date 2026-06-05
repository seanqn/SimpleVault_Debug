#include "vaultmanager.h"
#include "data/repository.h"

// TODO: CredentialModel and Repository incomplete

// TODO: QSortFilterProxyModel can filter vault content based on group_id via the groups model without needing to restructure how the data is retrieved from the repository

// REFACTOR: removing read operations away from controller/repoository communication and just letting the repository handle model read updates via signals
VaultManager::VaultManager(QObject *parent, Repository *repository, const QString &databaseName)
    : QObject(parent), m_repository(repository, databaseName) {

    m_groupsModel = new GroupsModel(this);
    m_credentialModel = new CredentialModel(this);

    initRepository();

    connect(m_repository, &Repository::groupAdded, m_groupsModel, &GroupsModel::append);
    connect(m_repository, &Repository::groupRemoved, m_groupsModel, &GroupsModel::remove);
    connect(m_repository, &Repository::groupRenamed, m_groupsModel, &GroupsModel::rename);

    // connect(m_repository, &Repository::credentialEntryAdded, m_credentialModel, &CredentialModel::append);
    // connect(m_repository, &Repository::credentialEntryRemoved, m_credentialModel, &CredentialModel::remove);
    // connect(m_repository, &Repository::credentialColumnModified, m_credentialModel, &CredentialModel::modify);

    updateGroups();
}

void VaultManager::initRepository() {
    if (!m_repository->initDatabase()) emit repositoryInitializationError();
}

// ideally should only be called by the constructor and other members
// void VaultManager::updateGroups() {
//     QList<Group> groups = m_repository->fetchGroups();
//     if (groups.isEmpty()) {
//         return;
//     }

//     m_groupsModel->update(groups);
// }

// database write entry is added to database and only appended to the cache
void VaultManager::createGroup(const QString &name) {
    Group newGroup = m_repository->addGroup(name);
    if (newGroup.id == 0) {
        emit createGroupError();
        return;
    }

    m_groupsModel->append(newGroup);
}

void VaultManager::selectGroup(int groupID) {
    m_currentGroupID = groupID;
    QList<Credential> credentials = m_repository->fetchCredentials(groupID);

    if (credentials.isEmpty()) {
        return;
    }

    m_credentialModel->update(credentials);
}

// rename will be selected via a right-click context menu option, which is not necessarily reflective of the current (double-click selected) group id
// QML will know what group is actually in context since it will be assigned as the current index, and then gets the group id at that index so that it is updated in the database
void VaultManager::renameGroup(int index, const QString &newName) {
    // get the id of that group in the respective cache
    Group groupAtIndex = m_repository->getGroupAt(index);
    groupAtIndex = m_repository->renameGroup(groupAtIndex, newName);
    if (groupAtIndex.name == newName) {
        m_groupsModel->replaceName(index, groupAtIndex);
        emit groupRenamed(groupAtIndex.id, newName);
    }
    emit groupRenameError(groupAtIndex.id, groupAtIndex.name);
}

void VaultManager::removeGroup(int index, int groupID) {
    QList<Group> updatedGroups = m_repository->removeGroup(index, groupID);
    m_groupsModel->update(updatedGroups);
}