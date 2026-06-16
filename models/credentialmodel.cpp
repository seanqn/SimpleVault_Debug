#include "credentialmodel.h"

CredentialModel::CredentialModel(QObject *parent)
    : QAbstractListModel(parent) {}

// refreshes the UI displaying updated credentials
void CredentialModel::update(const QList<Credential> &credentials) {
    beginResetModel();
    m_list = credentials;
    endResetModel();
}

void CredentialModel::appendRow(const Credential &row) {
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_list.append(row);
    endInsertRows();
}

void CredentialModel::syncNewRow(const Credential &row) {
    for (int i = 0; i < m_list.size(); ++i) {
        if (m_list[i].content_id == 0) {
            m_list[i] = row;
            QModelIndex modelIndex = createIndex(i, 0);
            emit dataChanged(modelIndex, modelIndex, {});
            return;
        }
    }

    appendRow(row);
}

void CredentialModel::updateRow(const Credential &row) {
    for (int i = 0; i < m_list.size(); ++i) {
        if (m_list[i].content_id == row.content_id) {
            m_list[i] = row;
            QModelIndex modelIndex = createIndex(i, 0);
            // third argument left as empty vector to signify (potentially) all roles have changed
            emit dataChanged(modelIndex, modelIndex, {});
            return;
        }
    }
}

void CredentialModel::removeRow(int index) {
    if (index < 0 || index >= m_list.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_list.removeAt(index);
    endRemoveRows();
}

QVariant CredentialModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size()) return QVariant();

    const Credential &credential = m_list.at(index.row());

    switch (role) {
    case ContentIDRole:
        return credential.content_id;
    case OrganizationRole:
        return credential.org_name;
    case UsernameRole:
        return credential.username;
    case PasswordRole:
        return credential.password;
    case EmailRole:
        return credential.email;
    case NoteRole:
        return credential.note;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> CredentialModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ContentIDRole] = "content_id";
    roles[OrganizationRole] = "org_name";
    roles[UsernameRole] = "username";
    roles[PasswordRole] = "password";
    roles[EmailRole] = "email";
    roles[NoteRole] = "note";
    return roles;
}