cmake_minimum_required(VERSION 3.16)

project(SimpleVault VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Qt6_DIR "/Users/shonny/Qt/6.9.3/macos/lib/cmake/Qt6")
set(BUILD_WITH_QT6 ON CACHE BOOL "Force Qt6" FORCE)
set(Qt5Core_FOUND FALSE CACHE BOOL "" FORCE)
set(Qt5_FOUND FALSE CACHE BOOL "" FORCE)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Sql Test)
# enable_testing()

list(APPEND CMAKE_PREFIX_PATH /Users/shonny/Qt/6.9.3/macos/lib/cmake)

qt_standard_project_setup(REQUIRES 6.8)

add_executable(SimpleVault
    app/main.cpp
)

# FIX: compiler is not recognizing includes in some (all headers involved with this test target) when attempting to add test
# add_executable(Test_VaultManager
#     controller/tests_controller.cpp
#     controller/vaultmanager.cpp
#     services/credentialeditcache.cpp
# )

# QML_IMPORT_PATH("/Users/shonny/Qt/6.9.3/macos/qml")

target_sources(SimpleVault PRIVATE
    data/credentialtypes.h
    data/repositories/db_local.h data/repositories/db_local.cpp
    data/repository.h data/repository.cpp
    models/credentialmodel.h models/credentialmodel.cpp
    models/groupsmodel.h models/groupsmodel.cpp
    services/credentialeditcache.h services/credentialeditcache.cpp
)

# after debug is finished, review and keep only necessary sources
qt_add_qml_module(SimpleVault
    URI SimpleVault
    VERSION 1.0
    QML_FILES
        views/Main.qml
        views/home/Home.qml
        views/auth/AuthStack.qml
        views/auth/AuthForm.qml
        views/auth/RegisterForm.qml
        views/home/components/HomePane.qml
        tests/groups_vaultcontent_tests.qml
    SOURCES
        services/vaultsecurityservicer.h services/vaultsecurityservicer.cpp
        controller/vaultmanager.h controller/vaultmanager.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(SimpleVault PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/config/Info.plist
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

# set(MY_ENTITLEMENTS ${CMAKE_CURRENT_SOURCE_DIR}/config/entitlements.plist)

# add_custom_command(TARGET SimpleVault POST_BUILD
#     COMMAND codesign --force --deep --sign --entitlements ${MY_ENTITLEMENTS} $<TARGET_BUNDLE_DIR:SimpleVault>
#     COMMENT "Applying Ad-Hoc signing and keychain entitlements"
# )

# QT KEYCHAIN
include(FetchContent)
FetchContent_Declare(
    qtkeychain
    GIT_REPOSITORY https://github.com/frankosterfeld/qtkeychain
    GIT_TAG ad7344c45a86a4f66cbafc4b081b5f7b876cb0b7 # v0.15.0
)
FetchContent_MakeAvailable(qtkeychain)
#

# LIBSODIUM
# set source
set(LIBSODIUM_INSTALL_DIR ${CMAKE_BINARY_DIR}/libsodium-install)
FetchContent_Declare(
    libsodium_src
    URL https://download.libsodium.org/libsodium/releases/libsodium-1.0.22.tar.gz
    URL_HASH SHA256=adbdd8f16149e81ac6078a03aca6fc03b592b89ef7b5ed83841c086191be3349
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(libsodium_src)

# build external project (library itself does not build with CMAKE)
include(ExternalProject)
ExternalProject_Add(
    libsodium_ext
    SOURCE_DIR ${libsodium_src_SOURCE_DIR}
    PREFIX ${CMAKE_BINARY_DIR}/libsodium
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${LIBSODIUM_INSTALL_DIR} --disable-shared --enable-static
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
    TEST_COMMAND ""
)

# import targets with existing source
add_library(libsodium STATIC IMPORTED GLOBAL)
set_target_properties(libsodium PROPERTIES
    IMPORTED_LOCATION ${LIBSODIUM_INSTALL_DIR}/lib/libsodium.a
    # must use the include directories to quiet the compiler
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${LIBSODIUM_INSTALL_DIR}/include>
)
add_dependencies(libsodium libsodium_ext)
#

target_include_directories(SimpleVault PRIVATE
    ${qtkeychain_BINARY_DIR}
    ${qtkeychain_SOURCE_DIR}
)

target_link_libraries(SimpleVault PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Sql
    Qt6::Test
    qt6keychain
    libsodium
)

# add_test(NAME Test_VaultManager COMMAND Test_VaultManager)

include(GNUInstallDirs)

install(TARGETS SimpleVault
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
