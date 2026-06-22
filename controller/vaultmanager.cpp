#include "vaultmanager.h"
#include "data/repository.h"

// TODO: CredentialModel and Repository incomplete

// TODO: QSortFilterProxyModel can filter vault content based on group_id via the groups model without needing to restructure how the data is retrieved from the repository

VaultManager::VaultManager(QObject *parent, const QString &databaseName)
    : QObject(parent) {

    m_repository = new Repository(this, databaseName);

    m_groupsModel = new GroupsModel(this);
    m_credentialModel = new CredentialModel(this);

    initRepository();

    connect(m_repository, &Repository::groupCacheEmpty, this, &VaultManager::repositoryGroupCacheEmpty);
    connect(m_repository, &Repository::credentialCacheEmpty, this, &VaultManager::repositoryCredentialCacheEmpty);

    connect(m_repository, &Repository::groupCacheUpdated, m_groupsModel, &GroupsModel::update);
    connect(m_repository, &Repository::groupEntryAdded, m_groupsModel, &GroupsModel::append);
    connect(m_repository, &Repository::groupEntryRemoved, m_groupsModel, &GroupsModel::remove);
    connect(m_repository, &Repository::groupEntryRenamed, m_groupsModel, &GroupsModel::rename);

    connect(m_repository, &Repository::credentialCacheUpdated, m_credentialModel, &CredentialModel::update);
    connect(m_repository, &Repository::newCredentialRowAdded, m_credentialModel, &CredentialModel::syncNewRow);
    connect(m_repository, &Repository::credentialRowUpdated, m_credentialModel, &CredentialModel::updateRow);

    // connect(m_repository, &Repository::credentialRowRemoved, m_credentialModel, &CredentialModel::removeRow);
}

void VaultManager::initRepository() {
    if (!m_repository->initDatabase())  {
        emit repositoryInitializationError();
        return;
    }
    m_repository->mockCredentialRow();
    updateGroups();
}

/*
groups model management methods
*/

// ideally should only be called by the constructor and other members
void VaultManager::updateGroups() {
    m_repository->fetchGroups();
}

// database write entry is added to database and only appended to the cache
void VaultManager::createGroup(const QString &name) {
    if (m_repository->addGroup(name)) {
        return;
    }

    emit createGroupError();
}

void VaultManager::selectGroup(int groupID) {
    // seems to be some discrepancy between actual group id in the db table and autoincremented model IDRole values
    m_repository->fetchCredentials(groupID);
    m_currentGroupID = groupID;
    emit groupChanged(m_currentGroupID);
}

// rename will be selected via a right-click context menu option, which is not necessarily reflective of the current (double-click selected) group id
// QML will know what group is actually in context since it will be assigned as the current index, and then gets the group id at that index so that it is updated in the database
void VaultManager::renameGroup(int groupID, const QString &newName) {
    if (m_repository->renameGroup(groupID, newName)) {
        emit groupRenamed(newName);
        return;
    }

    emit groupRenameError();
}

void VaultManager::removeGroup(int index, int groupID) {
    if (m_repository->removeGroup(index, groupID)) {
        emit groupRemoved(groupID);
        m_currentGroupID = 0;
        emit groupChanged(m_currentGroupID);
        return;
    }

    emit groupRemoveError(groupID);
}

/*
credential model management methods
*/

// adding a default row will switch active focus to it, requring QML to call startRowEdit
// if active focus is swithed again, submitRow is called
// per submitRow logic, a row that has been added without any changes made after editing is removed immediately
void VaultManager::addDefaultCredentialRow() {
    // prevents another empty row from being added if an empty row already exists
    if (m_editRowIndex == m_credentialModel->rowCount()) {
        qDebug() << "addDefaultCredentialRow: empty credential row already exists";
        return;
    }

    if (m_editRowIndex != -1) {
        submitRow();
    }

    Credential newRow{};
    m_credentialModel->appendRow(newRow);

    m_editRowIndex = m_credentialModel->rowCount() - 1;
    selectCredentialRow(m_editRowIndex);
}

// selection logic is vital since it also determines when the edit row index is reassigned
void VaultManager::selectCredentialRow(int rowIndex) {
    qDebug() << "[VaultManager]: selectCredentialRow called to select row with model index: " << rowIndex;
    startRowEdit(rowIndex);
}

void VaultManager::startRowEdit(int rowIndex) {
    if (rowIndex < 0 || rowIndex >= m_credentialModel->rowCount()) {
        qDebug() << "[VaultManager]: startRowEdit() called with row index: " << rowIndex << " out of range. Model row count: " << m_credentialModel->rowCount();
        return;
    }

    // prevents submitting the row when switching to different columns in the same row
    if (m_editRowIndex == rowIndex) return;

    // this method can be called directly by QML so this guard catches if another row was in edit and did not submit
    if (m_editRowIndex != -1) {
        qDebug() << "[VaultManager]: startRowEdit() submitting an unsubmitted row previously in edit for row index: " << m_editRowIndex;
        submitRow();
    }

    m_editRowIndex = rowIndex;
    m_rowCache = m_credentialModel->getCredentialAt(rowIndex);
    m_currentContentID = m_rowCache.content_id;
    qDebug() << "[VaultManager]: starting edit for model row: " << m_editRowIndex << ", with content id: " << m_currentContentID;
}

void VaultManager::updateRowCacheField(const QString &role, const QString &value) {
    if (m_editRowIndex == -1) return;

    m_rowCache.content_id = m_currentContentID;

    if (role == "org_name") {
        m_rowCache.org_name = value;
    }
    else if (role == "username") {
        m_rowCache.username = value;
    }
    else if (role == "password") {
        m_rowCache.password = value;
    }
    else if (role == "email") {
        m_rowCache.email = value;
    }
    else if (role == "note") {
        m_rowCache.note = value;
    }
    else {
        qDebug() << "updateRowCacheField: no " << role << " role found in row cache or invalid value " << value;
        return;
    }
}

void VaultManager::submitRow() {
    if (m_editRowIndex == -1) return;

    bool isEmpty = m_rowCache.org_name.isEmpty() &&
                   m_rowCache.username.isEmpty() &&
                   m_rowCache.password.isEmpty() &&
                   m_rowCache.email.isEmpty() &&
                   m_rowCache.note.isEmpty();

    if (isEmpty) {
        m_credentialModel->removeRow(m_editRowIndex);
        qDebug() << "submitRow: all values in row cache are after editing, new row has been removed";
    }
    else {
        qDebug() << "submitRow: calling repository->upsertCredentialRow for [group] " << m_currentGroupID
                 << ", row cache: [content id]: " << m_rowCache.content_id << ", [org_name]: " << m_rowCache.org_name
                 << ", [username]: " << m_rowCache.username << " [password]: " << m_rowCache.password
                 << ", [email]: " << m_rowCache.email << " [note]: " << m_rowCache.note;
        m_repository->upsertCredentialRow(m_currentGroupID, m_rowCache);
    }

    m_editRowIndex = -1;
}
