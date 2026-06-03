#ifndef CREDENTIALSMODEL_H
#define CREDENTIALSMODEL_H

#include <QAbstractListModel>
#include "data/credentialtypes.h"

// models are passed via QML property macros in the controller class
// note that this and the GroupsModel class previously used QML_ANONYMOUS but was creating some redundancies this way
class CredentialModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit CredentialModel(QObject *parent = nullptr);
    enum CredentialRoles {
        GroupIDRole = Qt::UserRole + 1,
        ContentIDRole,
        OrganizationRole,
        UsernameRole,
        PasswordRole
    };
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    // void setCredentials(const QList<Credential> &credentials);
    void update(const QList<Credential> &data);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_list.size(); }

private:
    QList<Credential> m_list;
};

#endif // CREDENTIALSMODEL_H
