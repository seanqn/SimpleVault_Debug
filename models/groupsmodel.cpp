#include "groupsmodel.h"

// data() was updated to return Group struct members

GroupsModel::GroupsModel(QObject *parent)
    : QAbstractListModel(parent) {}

// update() is when the model needs to be populated with the groups database table during builds
void GroupsModel::update(QList<Group> &groups) {
    beginResetModel();
    m_list = groups;
    endResetModel();
}

void GroupsModel::append(Group &group) {
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_list.append(group);
    endInsertRows();
}

// replacement is only relevant to a groups Group.name member
// TODO: update rename method to accept index for instant lookup
void GroupsModel::rename(int id, const QString &name) {
    for (int i = 0; i < m_list.size(); ++i) {
        if (m_list[i].id == id) {
            m_list[i].name = name;
            QModelIndex modelIndex = createIndex(i, 0);
            emit dataChanged(modelIndex, modelIndex, {GroupNameRole});
            return;
        }
    }
}

void GroupsModel::remove(int index) {
    // index could not be out of range, standard guard
    if (index < 0 || index >= m_list.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_list.removeAt(index);
    endRemoveRows();
}

QHash<int, QByteArray> GroupsModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IDRole] = "id";
    roles[GroupNameRole] = "name";
    roles[CreatedAtRole] = "created_at";
    return roles;
}

QVariant GroupsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())  return QVariant();

    const Group &group = m_list.at(index.row());

    switch (role) {
    case IDRole:
        return group.id;
    case GroupNameRole:
        return group.name;
    case CreatedAtRole:
        return group.created_at;
    default:
        return QVariant();
    }
}

