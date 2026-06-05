#ifndef GROUPSMODEL_H
#define GROUPSMODEL_H

#include <QAbstractListModel>
#include "data/credentialtypes.h"

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
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_list.size(); }
    void update(QList<Group> &groups);

public slots:
    void append(Group &group);
    void rename(int index, const QString &name);
    void remove(int index);

private:
    // the actual structure of this model essentially just serves as the destination for the repository group cache
    QList<Group> m_list;
};

#endif // GROUPSMODEL_H
