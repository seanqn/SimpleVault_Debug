#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H

class GroupsModel;
class CredentialModel;
class Repository;

class VaultManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(CredentialModel* model READ model CONSTANT)

public:
    explicit VaultManager(QObject* parent = nullptr);
    CredentialModel* model() const { return m_model; }
    Q_INVOKABLE void selectGroup(int groupID);

private:
    GroupsModel* m_groupmodel;
    CredentialModel* m_credmodel;
    Repository* m_repository;
};

#endif // VAULTMANAGER_H
