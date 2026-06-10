#include "credentialmodel.h"

CredentialModel::CredentialModel(QObject *parent)
    : QAbstractListModel(parent) {}

// TODO: move all non-overridden methods to controller

// void CredentialModel::setCredentials(const QList<Credential> &credentials) {
//     beginResetModel();
//     m_list = credentials;
//     endResetModel();
// }

// refreshes the UI displaying updated credentials
void CredentialModel::update(const QList<Credential> &credentials) {
    beginResetModel();
    m_list = credentials;
    endResetModel();
}

void CredentialModel::appendRow() {
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    Credential tmpRow;
    m_list.append(tmpRow);
    endInsertRows();
}

// void CredentialModel::modifyColumn(int contentID, int column, const QString &credential) {
//     if (column < 2 || column > 4 || contentID >= m_list.size()) {
//         return;
//     }

//     for (int i = 0; i < m_list.size(); ++i) {
//         if (m_list[i].content_id == contentID) {
//             switch(column) {
//             case 2:
//                 m_list[i][column].org_name = credential;
//                 const QString role = OrganizationRole;
//             case 3:
//                 m_list[i][column].username = credential;
//                 const QString role = UsernameRole;
//             case 4:
//                 m_list[i][column].password = credential;
//                 const QString role = PasswordRole;
//             default:
//                 return;
//             }

//             QModelIndex modelIndex = createIndex(i, 0);
//             emit dataChanged(modelIndex, modelIndex, {role})
//         }
//     }
// }

QVariant CredentialModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size()) return QVariant();

    const Credential &credential = m_list.at(index.row());

    switch (role) {
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
    roles[OrganizationRole] = "org_name";
    roles[UsernameRole] = "username";
    roles[PasswordRole] = "password";
    return roles;
}