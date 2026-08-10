#include <QtTest>
#include "vaultmanager.h"

class TestController : public QObject {
    Q_OBJECT

private slots:
    void initCase();
    void cleanupCase();
    void init();
    void cleanup();

    void test_startRowEdit(int index);
    void test_updateEditCache(const QString &role, const QString &value);
    void test_submitAndResetEditCache();
    void test_checkEditCacheEmpty();
    void test_checkEditCacheClean();
};

void TestController::initCase() {

}

void TestController::cleanupCase() {

}

void TestController::init() {

}

void TestController::cleanup() {

}

void TestController::test_startRowEdit(int index) {

}

QTEST_MAIN(TestController)
#include "tests_controller.moc"