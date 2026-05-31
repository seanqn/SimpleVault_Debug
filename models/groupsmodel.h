#ifndef GROUPSMODEL_H
#define GROUPSMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

// models are passed via QML property macros in the controller class and registers it as type for QML while staying private
class GroupsModel : public QAbstractListModel {
    Q_OBJECT
    QML_ANONYMOUS

public:
    enum GroupRoles {
        IDRole = Qt::UserRole + 1,
        GroupNameRole,
        CreatedAtRole
    };
    explicit GroupsModel(QObject *parent = nullptr);
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole)const override;
    void setCurrentGroupID(int id);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return IDRole; }

private:
    int m_currentGroupID;
};

#endif // GROUPSMODEL_H
