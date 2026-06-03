#ifndef GROUPSMODEL_H
#define GROUPSMODEL_H

#include <QAbstractListModel>
#include <data/credentialtypes.h>

// TODO: sruct member added, needs to be checked

// models are passed via QML property macros in the controller class and registers it as type for QML while staying private
class GroupsModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum GroupRoles {
        IDRole = Qt::UserRole + 1,
        GroupNameRole,
        CreatedAtRole
    };
    explicit GroupsModel(QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
    void update(QList<Group> &groups);
    void append(Group &group);
    void setCurrentGroupID(int id);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_list.size(); }

private:
    int m_currentGroupID;
    QList<Group> m_list;
};

#endif // GROUPSMODEL_H
