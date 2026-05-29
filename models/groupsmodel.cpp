#include "groupsmodel.h"

GroupsModel::GroupsModel(QObject* parent) {}

QHash<int, QByteArray> GroupsModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IDRole] = "id";
    roles[GroupNameRole] = "name";
    roles[CreatedAtRole] = "created_at";
    return roles;
}

QVariant GroupsModel::data(const QModelIndex &index, int role) const override {
    if (!index.isValid() || index.row() < 0)  return QVariant();

    switch (role) {
    case IDRole:
        return 0;
    case GroupNameRole:
        return 1;
    case CreatedAtRole:
        return 2;
    default:
        return QVariant();
    }
}

