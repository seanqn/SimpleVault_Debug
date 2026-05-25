#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQmlContext>
#include "services/vaultsecurityservicer.h"
#include "controller/vaultmanager.h"

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

    VaultManager *vaultManager = nullptr;
    if (runInTest) {
        vaultManager = new VaultManager(nullptr, ":memory:");
    }
    else {
        vaultManager = new VaultManager(nullptr);
    }

    // initialize class

    // primary controller
    // instantiates DB_LocalStorage, GroupsModel

    qmlRegisterSingletonInstance("VaultManager", 1, 0, "VaultManager", vaultManager);

    // about model and database:
    // The custom defined DB_LocalStorage class contain read/write/update methods
    // GroupsModel, VaultContentModel derived from QSqlTableModel is read only but contains data from the core db instance

    // authentication servicer
    VaultSecurityServicer *vaultAuthenticator = new VaultSecurityServicer();
    qmlRegisterSingletonInstance("VaultSecurityServicer", 1, 0, "Authenticator", vaultAuthenticator);

    // local database
    // DB_LocalStorage *vaultDB = new DB_LocalStorage();
    // QObject::connect(vaultDB, &DB_LocalStorage::directoryEstablished, [](QString path){
    //     qDebug() << "DB Path established: " << path;
    // });
    // qmlRegisterSingletonInstance("DB_LocalStorage", 1, 0, "VaultDB", vaultDB);


    // start qml connection

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // if (vaultAuthenticator->m_uid.isEmpty()) {
    //         // registration qml
    //     }
    // else {
    //         // authentication qml
    //     }

    engine.rootContext()->setContextProperty("vaultManager", vaultManager);
    if (testCase == 1) {
        engine.load(QUrl(QStringLiteral("qrc:/qt/qml/SimpleVault/tests/groups_vaultcontent_tests.qml")));
    }
    else {
        engine.load(QUrl(QStringLiteral("qrc:/qt/qml/SimpleVault/views/Main.qml")));
    }

    return app.exec();
}
