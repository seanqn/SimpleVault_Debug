#ifndef CREDENTIALSMODEL_H
#define CREDENTIALSMODEL_H

#include <QAbstractListModel>

struct Credential;

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
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const override;
    void setCredentials(const QList<Credential> &credentials);
    void update(QList<QVariant> &data);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_list.size(); }

private:
    QList<Credential> m_list;
};

#endif // CREDENTIALSMODEL_H
