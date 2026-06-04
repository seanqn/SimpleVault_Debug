#include "vaultmanager.h"
#include "data/repository.h"

// TODO: CredentialModel and Repository incomplete

// TODO: QSortFilterProxyModel can filter vault content based on group_id via the groups model without needing to restructure how the data is retrieved from the repository

VaultManager::VaultManager(QObject *parent, const QString &databaseName) {
    m_groupsModel = new GroupsModel(this);
    m_credentialModel = new CredentialModel(this);
    m_repository = new Repository(this, databaseName);

    initRepository();
    updateGroups();
}

void VaultManager::initRepository() {
    if (!m_repository->initDatabase()) emit repositoryInitializationError();
}

// ideally should only be called by the constructor and other members
void VaultManager::updateGroups() {
    QList<Group> groups = m_repository->fetchGroups();
    if (groups.isEmpty()) {
        emit groupsUpdateError();
        return;
    }

    m_groupsModel->update(groups);
}

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
    QList<Credential> data = m_repository->fetchCredentials(groupID);
    m_credentialModel->update(data);
    m_currentGroupID = groupID;
    m_groupsmodel.setCurrentGroupID(groupID);
}

// QML could accept a valid new group name and let it be portrayed on the listview without bothering to update the model until the next
// model will accurately update to the new name when it's retrieved from the database each build
void VaultManager::renameGroup(int index, const QString &newName) {
}

void VaultManager::removeGroup(int index, int groupID) {
    QList<Group> updatedGroups = m_repository->removeGroup(index, groupID);
    m_groupsModel->update(updatedGroups);
}