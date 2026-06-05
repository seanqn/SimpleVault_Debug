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
    beginResetModel();
    m_list.append(group);
    endResetModel();
}

// replacement is only relevant to a groups Group.name member
void GroupsModel::rename(int index, const QString &name) {
    if (m_list.size() <= index) {
        beginResetModel();
        m_list.at(index).name.assign(name);
        endResetMode();
    }
}

void GroupsModel::remove(int index) {
    if (m_list.size() <= index) {
        beginResetModel();
        m_list.remove(index);
        endResetModel();
    }
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

