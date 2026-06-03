#include "credentialmodel.h"

CredentialModel::CredentialModel(QObject *parent)
    : QAbstractListModel(parent) {}

// TEST: remove redundant setCredentials() and move logic to update(), whichever is called primarily has a const type paramater

// void CredentialModel::setCredentials(const QList<Credential> &credentials) {
//     beginResetModel();
//     m_list = credentials;
//     endResetModel();
// }

// refreshes the UI displaying updated credentials
void CredentialModel::update(const QList<Credential> &data) {
    // previous function logic
    // setCredentials(data);

    // updated function logic (formerly setCredentials())
    beginResetModel();
    m_list = data;
    endResetModel();
}

QVariant CredentialModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size()) return QVariant();

    const Credential &credential = m_list.at(index.row());

    switch (role) {
    case GroupIDRole:
        return credential.group_id;
    case ContentIDRole:
        return credential.content_id;
    case OrganizationRole:
        return credential.org_name;
    case UsernameRole:
        return credential.username;
    case PasswordRole:
        return credential.password;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> CredentialModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[GroupIDRole] = "group_id";
    roles[ContentIDRole] = "content_id";
    roles[OrganizationRole] = "org_name";
    roles[UsernameRole] = "username";
    roles[PasswordRole] = "password";
    return roles;
}