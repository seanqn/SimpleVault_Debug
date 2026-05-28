#ifndef CREDENTIALSMODEL_H
#define CREDENTIALSMODEL_H

#include <QAbstractListModel>
#include <data/credentialtypes.h>

// TODO: add definitions to source

class CredentialModel : QAbstractListModel {
    Q_OBJECT

public:
    explicit CredentialModel(QObject* parent = nullptr);

    enum CredentialRoles {
        GroupIDRole = Qt::UserRole + 1,
        OrganizationRole,
        UsernameRole,
        Passwordrole
    };

    void setCredentials(const QList<Credential> &credentials) {
        beginResetModel();
        m_list = credentials;
        endResetModel();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_list.size(); }

    update(QList<QVariant> &data);

    QVariant data(const QModelIndex &index, int role) const override {
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

    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[GroupIDRole] = "group_id";
        roles[OrganizationRole] = "org_name";
        roles[UsernameRole] = "username";
        roles[Passwordrole] = "password";
        return roles;
    }

private:
    QList<Credential> m_list;
};

#endif // CREDENTIALSMODEL_H
