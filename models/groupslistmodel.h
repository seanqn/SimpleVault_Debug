#ifndef GROUPSLISTMODEL_H
#define GROUPSLISTMODEL_H

#include <QAbstractListModel>

class GroupsListModel : QAbstractListModel {
    Q_OBJECT

public:
    explicit GroupsListModel(QObject* parent = nullptr, const QString &repository);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // GROUPSLISTMODEL_H
