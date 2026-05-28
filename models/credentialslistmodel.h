#ifndef CREDENTIALSLISTMODEL_H
#define CREDENTIALSLISTMODEL_H

#include <QAbstractListModel>

class CredentialsListModel : QAbstractListModel {
    Q_OBJECT

public:
    explicit CredentialsListModel(QObject* parent = nullptr, const QString &repository);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};


#endif // CREDENTIALSLISTMODEL_H
