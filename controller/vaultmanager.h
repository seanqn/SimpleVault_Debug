#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H
// #include "data/repositories/db_local.h"
// #include "models/groupsmodel.h"
// #include "models/vaultcontentmodel.h"
// #include "data/repository.h"
class CredentialsModel;
class Repository;

class VaultManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(CredentialsModel* model READ model CONSTANT)

public:
    explicit VaultManager(QObject* parent = nullptr);
    CredentialsModel* model() const { return m_model; }
    Q_INVOKABLE void selectGroup(int groupID);

private:
    CredentialsModel* m_model;
    Repository* m_repository;
};

#endif // VAULTMANAGER_H
