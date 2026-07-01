#include "vaultmanager.h"
#include "data/repository.h"

// TODO: CredentialModel and Repository incomplete

// TODO: QSortFilterProxyModel can filter vault content based on group_id via the groups model without needing to restructure how the data is retrieved from the repository

VaultManager::VaultManager(QObject *parent, const QString &databaseName)
    : QObject(parent) {

    m_repository = new Repository(this, databaseName);
    connect(m_repository, &Repository::groupCacheEmpty, this, &VaultManager::repositoryGroupCacheEmpty);
    connect(m_repository, &Repository::credentialCacheEmpty, this, &VaultManager::repositoryCredentialCacheEmpty);

    m_groupsModel = new GroupsModel(this);
    connect(m_repository, &Repository::groupCacheUpdated, m_groupsModel, &GroupsModel::update);
    connect(m_repository, &Repository::groupEntryAdded, m_groupsModel, &GroupsModel::append);
    connect(m_repository, &Repository::groupEntryRemoved, m_groupsModel, &GroupsModel::remove);
    connect(m_repository, &Repository::groupEntryRenamed, m_groupsModel, &GroupsModel::rename);

    m_credentialModel = new CredentialModel(this);
    connect(m_repository, &Repository::credentialCacheUpdated, m_credentialModel, &CredentialModel::update);
    // connect(m_repository, &Repository::newCredentialRowAdded, m_credentialModel, &CredentialModel::syncNewRow);
    // connect(m_repository, &Repository::credentialRowUpdated, m_credentialModel, &CredentialModel::updateRow);

    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setSingleShot(true);
    m_autoSaveTimer->setInterval(1500);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &VaultManager::commitEditCacheToDraft);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &VaultManager::relayAutoSaveTimeout);

    initRepository();
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
    if (m_editRowIndex != -1) {
        commitEditCacheToDraft();
    }

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

// invokable edit cache verification methods
// credentialRowIsEmpty tracks either a default row that was submitted without any changes or a row where all fields were cleared
bool VaultManager::editCacheIsEmpty() {
    bool isEmpty = m_editCache.org_name.isEmpty() &&
                   m_editCache.username.isEmpty() &&
                   m_editCache.password.isEmpty() &&
                   m_editCache.email.isEmpty() &&
                   m_editCache.note.isEmpty();
    return isEmpty;
}

// credentialRowIsClean tracks any diffs
bool VaultManager::editCacheIsClean() {
    bool isClean = (m_editCache.org_name == m_row.org_name &&
                    m_editCache.username == m_row.username &&
                    m_editCache.password == m_row.password &&
                    m_editCache.email == m_row.email &&
                    m_editCache.note == m_row.note);
    return isClean;
}

// per submitRow logic, a row that has been added without any changes made after editing is removed immediately
// ideally starts editing after added, but cannot conflict with the startRowEdit logic (if m_editRowIndex is set here, any rows that were previously in edit will not be submitted and retain old values)
// could pass the last model index to startRowEdit, but the active focus still needs to be true in QML
void VaultManager::addDefaultCredentialRow() {
    // prevents another empty row from being added if an empty row already exists
    int listSize = m_credentialModel->rowCount();
    if (listSize > 0) {
        if (m_credentialModel->getCredentialAt(listSize - 1).content_id == 0) {
            qDebug() << "new row exists. no action done";
            return;
        }
    }

    if (m_editRowIndex != -1 && !(editCacheIsClean())) {
        submitAndResetEditCache();
    }

    Credential newRow{};
    m_credentialModel->appendRow(newRow);
}

void VaultManager::selectCredentialRow(int rowIndex) {
    qDebug() << "[VaultManager]: selectCredentialRow called to select row with model index: " << rowIndex;
}

void VaultManager::startRowEdit(int rowIndex) {
    if (rowIndex < 0 || rowIndex >= m_credentialModel->rowCount()) {
        qDebug() << "[VaultManager]: startRowEdit() called with row index: " << rowIndex << " out of range. Model row count: " << m_credentialModel->rowCount() - 1;
        return;
    }

    // prevents submitting the row when switching to different columns in the same row
    if (m_editRowIndex == rowIndex) return;

    // this method can be called directly by QML so this guard catches if another row was in edit and did not submit
    if (m_editRowIndex != -1) {
        qDebug() << "[VaultManager]: startRowEdit() submitting an unsubmitted row previously in edit for row index: " << m_editRowIndex;
        submitAndResetEditCache();
    }

    m_editRowIndex = rowIndex;
    // stores the existing row fields before updating
    m_row = m_credentialModel->getCredentialAt(rowIndex);
    m_editCache = m_row;
    m_currentContentID = m_row.content_id;
    qDebug() << "[VaultManager]: starting edit for model row: " << m_editRowIndex << ", with content id: " << m_currentContentID;
}

void VaultManager::updateEditCache(const QString &role, const QString &value) {
    if (m_editRowIndex == -1) return;

    if (role == "org_name") {
        m_editCache.org_name = value;
    }
    else if (role == "username") {
        m_editCache.username = value;
    }
    else if (role == "password") {
        m_editCache.password = value;
    }
    else if (role == "email") {
        m_editCache.email = value;
    }
    else if (role == "note") {
        m_editCache.note = value;
    }
    else {
        qDebug() << "updateRowCacheField: no " << role << " role found in edit cache or invalid value " << value;
        return;
    }

    qDebug() << "[VaultManager]: updatedRowCacheField updated role " << role << " to " << value;
}

// when a single column has finished editing, reset the timer
void VaultManager::resetAutoSaveTimer() {
    if (m_editRowIndex != -1) {
        m_autoSaveTimer->start();
        qDebug() << "[VaultManager]: autosave: timer start/reset";
    }
}

void VaultManager::stopAutoSaveTimer() {
    if (m_autoSaveTimer->isActive()) {
        m_autoSaveTimer->stop();
        qDebug() << "[VaultManager]: autosave: timer force stopped";
    }
}

void VaultManager::relayAutoSaveTimeout() {
    qDebug() << "[VaultManager]: autosave: timer timed out";
}

void VaultManager::commitEditCacheToDraft() {
    // when a column has finished editing, commit to the draft
    // the draft may be a separate file (JSON or other)
}

// called when the edit index has changed or active focus loss from editing row
void VaultManager::submitAndResetEditCache() {
    if (m_editRowIndex == -1) return;

    qDebug() << "[VaultManager]: performing submit and reset edit cache";
    // the timer will submit row if hits timeout, but manually stopping timer requires manual submission
    if (editCacheIsEmpty()) {
        m_credentialModel->removeRow(m_editRowIndex);
        qDebug() << "all values in row cache are empty after editing, new row has been removed";
    }
    else if (editCacheIsClean()) {
        qDebug() << "edit cache is clean, no action done";
    }
    else {
        qDebug() << "calling repository->upsertCredentialRow for [group] " << m_currentGroupID
                 << ", edit cache: [content id]: " << m_editCache.content_id << ", [org_name]: " << m_editCache.org_name
                 << ", [username]: " << m_editCache.username << " [password]: " << m_editCache.password
                 << ", [email]: " << m_editCache.email << " [note]: " << m_editCache.note;
        m_repository->upsertCredentialRow(m_currentGroupID, m_editCache);
    }

    m_editRowIndex = -1;
    m_row = Credential{};
    m_editCache = Credential{};
    qDebug() << "[VaultManager]: edit cache reset";
}
