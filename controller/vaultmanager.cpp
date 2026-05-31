#include "vaultmanager.h"
#include "models/groupsmodel.h"
#include "models/credentialmodel.h"
#include "data/repository.h"

// TODO: CredentialModel and Repository incomplete

VaultManager::VaultManager(QObject *parent, const QString &databaseName) {
    m_groupsModel = new GroupsModel(this);
    m_credentialMmodel = new CredentialModel(this);
    m_repository = new Repository(this, databaseName);
}

// retrieves the cache of all rows belonging to the selected group
// cache is then passed along to the model for presentation
void VaultManager::createGroup(const QString &name) {
    // tell database via repository to add a group to the groups table and return the id upon success
    if (m_repository->addGroup(name)) return;
    emit createGroupError();
}

// see if the group id setter should be implemented as a member of the groups model or stay in the controller
void VaultManager::selectGroup(int groupID) {
    QList<Credential> data = m_repository->fetchCredentials(groupID);
    m_credentialModel->update(data);
    m_currentGroupID = groupID;
    // m_groupsmodel.setCurrentGroupID(groupID);
}


