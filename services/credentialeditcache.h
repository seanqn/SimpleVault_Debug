#ifndef EDITCACHE_H
#define EDITCACHE_H

#include <QObject>
#include "models/credentialmodel.h"

class CredentialEditCache : public QObject
{
    Q_OBJECT
public:
    explicit EditCache(QObject *parent = nullptr)
        : QObject(parent) {
        m_credentialModel = model;
    }

    ~EditCache() {
        if (m_editRowIndex != -1) {
            submitAndResetEditCache();
        }
    }

    int getEditRowIndex() const { return m_editRowIndex; }

    bool isEmpty() {
        bool isEmpty = m_editCache.org_name.isEmpty() &&
                       m_editCache.username.isEmpty() &&
                       m_editCache.password.isEmpty() &&
                       m_editCache.email.isEmpty() &&
                       m_editCache.note.isEmpty();
        return isEmpty;
    }

    bool isClean() {
        bool isClean = (m_editCache.org_name == m_row.org_name &&
                        m_editCache.username == m_row.username &&
                        m_editCache.password == m_row.password &&
                        m_editCache.email == m_row.email &&
                        m_editCache.note == m_row.note);
        return isClean;
    }

    int startRowEdit(int index, int rowCount, Credential &cred) {
        if (index < 0 || index >= rowCount || m_editRowIndex == index) return -1;

        if (m_editRowIndex != -1) {
            submitAndResetEditCache();
        }

        m_editRowIndex = index;
        m_row = cred;
        m_editCache = cred;
    }

    void updateEditCache(const QString &role, const QString &value) {
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
            return;
        }
    }

    bool submitAndResetEditCache() {
        if (m_editRowIndex == -1 || editCacheIsEmpty() || editCacheIsClean()) return false;

        if (defaultRowNotSubmitted) {
            defaultRowNotSubmitted = false;
        }

        m_editRowIndex = -1;
        m_row = Credential{};
        m_editCache = Credential{};

        return true;
    }


private:
    bool defaultRowNotSubmitted = false;
    int m_editRowIndex = -1;
    Credential m_row;
    Credential m_editCache;
};

#endif // EDITCACHE_H
