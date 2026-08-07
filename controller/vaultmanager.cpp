#include "vaultmanager.h"
#include "data/repository.h"

VaultManager::VaultManager(QObject *parent, const QString &databaseName)
    : QObject(parent) {

    m_repository = new Repository(this, databaseName);

    m_groupsModel = new GroupsModel(this);
    connect(m_repository, &Repository::groupCacheUpdated, m_groupsModel, &GroupsModel::update);
    connect(m_repository, &Repository::groupEntryAdded, m_groupsModel, &GroupsModel::append);

    m_credentialModel = new CredentialModel(this);
    connect(m_repository, &Repository::credentialCacheUpdated, m_credentialModel, &CredentialModel::update);

    initRepository();
}

VaultManager::~VaultManager() {
    if (m_editRowIndex != -1) {
        submitAndResetEditCache();
    }
}

void VaultManager::initRepository() {
    if (!m_repository->initDatabase())  {
        emit repositoryInitializationError();
        return;
    }
    m_repository->mockGroup();
    m_repository->fetchGroups();
}

/*
groups model management methods
*/

// group object instantiation lives here for now
void VaultManager::createGroup(const QString &name) {
    Group newGroup;
    newGroup.id = QUuid::createUuidV7().toRfc4122();
    newGroup.name = name;
    newGroup.created_at = QDateTime::currentDateTime();
    if (m_repository->addGroup(newGroup)) {
        return;
    }

    emit createGroupError();
}

void VaultManager::selectGroup(int index) {
    // ensure that the edit cache is reset before group switching
    if (m_editRowIndex != -1) {
        submitAndResetEditCache();
    }

    QByteArray gid = m_groupsModel->getGroupIDAt(index);
    if (gid.isEmpty()) {
        qDebug() << "[Vault Manager]: selectGroup: group list index out of range";
        return;
    }
    if (m_currentGroupID == gid) return;

    m_repository->fetchCredentials(gid);
    m_currentGroupID = gid;
    emit groupChanged(QUuid::fromRfc4122(m_currentGroupID).toString());
}

void VaultManager::renameGroup(int index, const QString &newName) {
    QByteArray gid = m_groupsModel->getGroupIDAt(index);
    if (gid.isEmpty()) {
        qDebug() << "[Vault Manager]: renameGroup: group list index out of range";
        return;
    }

    if (m_repository->renameGroup(gid, newName)) {
        m_groupsModel->rename(index, newName);
        emit groupRenamed(newName);
        return;
    }

    emit groupRenameError();
}

void VaultManager::removeGroup(int index) {
    QByteArray gid = m_groupsModel->getGroupIDAt(index);
    if (gid.isEmpty()) {
        qDebug() << "[Vault Manager]: removeGroup: group list index out of range";
        return;
    }

    if (m_repository->removeGroup(gid)) {
        m_groupsModel->remove(index);
        emit groupRemoved(QUuid::fromRfc4122(gid).toString());
        m_currentGroupID.clear();
        return;
    }

    emit groupRemoveError(QUuid::fromRfc4122(gid).toString());
}

/*
credential model management methods
*/

// invokable edit cache verification methods
bool VaultManager::editCacheIsEmpty() {
    bool isEmpty = m_editCache.org_name.isEmpty() &&
                   m_editCache.username.isEmpty() &&
                   m_editCache.password.isEmpty() &&
                   m_editCache.email.isEmpty() &&
                   m_editCache.note.isEmpty();
    return isEmpty;
}

bool VaultManager::editCacheIsClean() {
    bool isClean = (m_editCache.org_name == m_row.org_name &&
                    m_editCache.username == m_row.username &&
                    m_editCache.password == m_row.password &&
                    m_editCache.email == m_row.email &&
                    m_editCache.note == m_row.note);
    return isClean;
}

// credential object instantation lives here for now
void VaultManager::addDefaultCredentialRow() {
    // TODO: allow new rows as long as text for one column in the previous default row has been changed
    if (defaultRowNotSubmitted) {
        return;
    }

    if (m_editRowIndex != -1) {
        submitAndResetEditCache();
    }

    Credential newRow{};

    // new row key is created here as a 16 byte time-based uuid (must be 16 bytes to read as BLOB in db without conversion)
    newRow.content_id = QUuid::createUuidV7().toRfc4122();
    m_credentialModel->appendRow(newRow);
    defaultRowNotSubmitted = true;
}

void VaultManager::selectCredentialRow(int rowIndex) {
    qDebug() << "[VaultManager]: selectCredentialRow called to select row with model index: " << rowIndex;
}

void VaultManager::startRowEdit(int rowIndex) {
    if (rowIndex < 0 || rowIndex >= m_credentialModel->rowCount()) {
        qDebug() << "[VaultManager]: startRowEdit() called with row index: " << rowIndex << " out of range. Model row count: " << m_credentialModel->rowCount() - 1;
        return;
    }

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
    qDebug() << "[VaultManager]: starting edit for model row: " << m_editRowIndex << ", with content id: " << QUuid::fromRfc4122(m_row.content_id).toString();
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

// called when the edit index has changed or active focus loss from editing row
void VaultManager::submitAndResetEditCache() {
    if (m_editRowIndex == -1) return;

    qDebug() << "[VaultManager]: performing submit and reset edit cache";
    if (editCacheIsEmpty()) {
        m_credentialModel->removeRow(m_editRowIndex);
        qDebug() << "all values in row cache are empty after editing, new row has been removed";
    }
    else if (editCacheIsClean()) {
        qDebug() << "edit cache is clean, no action done";
    }
    else {
        qDebug() << "calling repository->upsertCredentialRow for [group] " << getCurrentGroupID()
                 << ", edit cache: [content id]: " << getCurrentContentID() << ", [org_name]: " << m_editCache.org_name
                 << ", [username]: " << m_editCache.username << " [password]: " << m_editCache.password
                 << ", [email]: " << m_editCache.email << " [note]: " << m_editCache.note;
        m_repository->upsertCredentialRow(m_currentGroupID, m_editCache);
        m_credentialModel->updateRow(m_editRowIndex, m_editCache);
    }

    if (defaultRowNotSubmitted) {
        defaultRowNotSubmitted = false;
    }
    m_editRowIndex = -1;
    m_currentContentID.clear();
    m_row = Credential{};
    m_editCache = Credential{};
    qDebug() << "[VaultManager]: edit cache reset";
}
