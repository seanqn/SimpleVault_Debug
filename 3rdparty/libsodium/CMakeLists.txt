# builds for libsodium

# collect src directory
file(GLOB_RECURSE SODIUM_SOURCES "src/libsodium/*.c")
add_library(libsodium STATIC ${SODIUM_SOURCES})
target_include_directories(libsodium PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/libsodium/include>
)

# define internal flags to avoid autoconf header reliance
target_compile_definitions(libsodium PRIVATE
    CONFIGURED=1
    DEV_MODE=1
)