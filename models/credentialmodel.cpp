#include "credentialmodel.h"
#include "data/credentialtypes.h"

CredentialModel::CredentialModel(QObject *parent) {}

void CredentialModel::setCredentials(const QList<Credential> &credentials) {
    beginResetModel();
    m_list = credentials;
    endResetModel();
}

// refreshes the UI dipslaying updated credentials
void CredentialModel::update(QList<Credential> &data) {

}

QVariant CredentialModel::data(const QModelIndex &index, int role) const override {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size()) return QVariant();

    const Credential &credential = m_list.at(index.row());

    switch (role) {
    case GroupIDRole:
        return credential.group_id;
    case OrganizationRole:
        return credential.org_name;
    case UsernameRole:
        return credential.username;
    case Passwordrole:
        return credential.password;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> CredentialModel::roleNames() const {
    QHash<int, QByteArray> role;

    QHash<int, QByteArray> roles;
    roles[GroupIDRole] = "group_id";
    roles[OrganizationRole] = "org_name";
    roles[UsernameRole] = "username";
    roles[Passwordrole] = "password";
    return roles;
}