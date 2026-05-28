#include "vaultmanager.h"
#include "models/credentialsmodel.h"
#include "data/repository.h"

// TODO: CredentialsModel is not complete

VaultManager::VaultManager(QObject *parent) {
    m_model = new CredentialsModel(this);
    m_repository = new Repository(this);
}

void VaultManager::selectGroup(int groupID) {
    QList<Credential> data = m_repository->fetchCredentials(groupID);
    m_model->update(data);
}


