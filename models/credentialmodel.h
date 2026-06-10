#ifndef CREDENTIALSMODEL_H
#define CREDENTIALSMODEL_H

#include <QAbstractListModel>
#include "data/credentialtypes.h"

class CredentialModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit CredentialModel(QObject *parent = nullptr);
    enum CredentialRoles {
        OrganizationRole,
        UsernameRole,
        PasswordRole,
        EmailRole,
        NoteRole
    };
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_list.size(); }
    // columnCount expects reference to a parent but it is unused, no argument required
    int columnCount(const QModelIndex&) const override { return 5; }

public slots:
    void update(const QList<Credential> &credentials);
    void appendRow();
    // void removeRow(int contentID);
    // void modifyColumn(int contentID, int column, const QString &credential);

private:
    QList<Credential> m_list;
};

#endif // CREDENTIALSMODEL_H
