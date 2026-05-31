#ifndef CREDENTIALSMODEL_H
#define CREDENTIALSMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

struct Credential;

// models are passed via QML property macros in the controller class and registers it as type for QML while staying private
class CredentialModel : QAbstractListModel {
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit CredentialModel(QObject *parent = nullptr);
    enum CredentialRoles {
        GroupIDRole = Qt::UserRole + 1,
        ContentIDRole,
        OrganizationRole,
        UsernameRole,
        Passwordrole
    };
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void setCredentials(const QList<Credential> &credentials);
    void update(QList<Credential> &data);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_list.size(); }

private:
    QList<Credential> m_list;
};

#endif // CREDENTIALSMODEL_H
