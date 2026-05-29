#ifndef GROUPSMODEL_H
#define GROUPSMODEL_H
#include <QSqlListModel>

// GroupsModel will overwrite current QML implementation in HomePane.qml

class GroupsModel : public QSqlTableModel {
    Q_OBJECT

public:
    enum GroupRoles {
        IDRole = Qt::UserRole + 1,
        GroupNameRole,
        CreatedAtRole
    };
    explicit GroupsModel(QObject* parent = nullptr, const QString &connectionName="SimpleVault");
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole)const override;
};

#endif // GROUPSMODEL_H
