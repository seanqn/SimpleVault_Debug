#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQmlContext>
#include "services/vaultsecurityservicer.h"
#include "controller/vaultmanager.h"
#include "data/repository.h"
#include "services/credentialeditcache.h"
// includes and sub-controller dependency creation will be consolidated into a primary controller later on

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("github/seanqn");
    QCoreApplication::setOrganizationDomain("com.seanqn.simplevault");
    QCoreApplication::setApplicationName("SimpleVault");
    QGuiApplication app(argc, argv);
    // QApplication::setOrganizationName("github/seanqn");

    bool runInTest = true;
    // testCases: [0] default, [1] qml, [2] backend
    int testCase = 1;

    Repository mainRepository(nullptr, runInTest ? ":memory:" : "SimpleVault");
    CredentialEditCache editCache;
    VaultManager vaultManager(mainRepository, editCache);

    // qmlRegisterSingletonInstance("VaultManager", 1, 0, "VaultManager", vaultManager);

    // authentication servicer
    VaultSecurityServicer *vaultAuthenticator = new VaultSecurityServicer();
    qmlRegisterSingletonInstance("VaultSecurityServicer", 1, 0, "Authenticator", vaultAuthenticator);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );

    // engine.rootContext()->setContextProperty("vaultManager", vaultManager);
    if (testCase == 1) {
        engine.load(QUrl(QStringLiteral("qrc:/qt/qml/SimpleVault/tests/groups_vaultcontent_tests.qml")));
    }
    else {
        engine.load(QUrl(QStringLiteral("qrc:/qt/qml/SimpleVault/views/Main.qml")));
    }

    return app.exec();
}
