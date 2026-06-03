#include "groupsmodel.h"

// data() was updated to return Group struct members

GroupsModel::GroupsModel(QObject *parent)
    : QAbstractListModel(parent) {}

// upate() and append() both existing is redundant, will want to consolidate to one

void GroupsModel::update(QList<Group> &groups) {
    beginResetModel();
    m_list = groups;
    endResetModel();
}

// check to make sure that appending will update the list view or if requires update()
void GroupsModel::append(Group &group) {
    beginResetModel();
    m_list.append(group);
    endResetModel();
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

void GroupsModel::setCurrentGroupID(int id) {
    m_currentGroupID = id;
}

