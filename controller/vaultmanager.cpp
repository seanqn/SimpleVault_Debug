#include "vaultmanager.h"
#include "models/groupsmodel.h"
#include "models/credentialmodel.h"
#include "data/repository.h"

// TODO: CredentialModel and Repository incomplete

VaultManager::VaultManager(QObject *parent) {
    m_model = new CredentialModel(this);
    m_repository = new Repository(this);
}

void VaultManager::selectGroup(int groupID) {
    QList<Credential> data = m_repository->fetchCredentials(groupID);
    m_model->update(data);
}


