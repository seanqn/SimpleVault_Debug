#include "groupsmodel.h"

GroupsModel::GroupsModel(QObject* parent, const QString &connectionName)
    : QSqlTableModel(parent, QSqlDatabase::database(connectionName))
{
    setTable("groups");
    setEditStrategy(QSqlTableModel::OnRowChange);

    setHeaderData(1, Qt::Horizontal, tr("name"));
    select();
}

QHash<int, QByteArray> GroupsModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IDRole] = "id";
    roles[GroupNameRole] = "name";
    roles[CreatedAtRole] = "created_at";
    return roles;
}

QVariant GroupsModel::data(const QModelIndex &index, int role) const {
    if (role < Qt::UserRole)  {
        return QSqlTableModel::data(index, role);
    }

    int column;
    if (role == IDRole) {
        column = 0;
    }
    else if (role == GroupNameRole) {
        column = 1;
    }
    else {
        column = 2;
    }
    return QSqlTableModel::data(this->index(index.row(), column));
}

