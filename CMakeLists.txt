cmake_minimum_required(VERSION 3.15)
project(mydiscord C)

set(CMAKE_C_STANDARD 99)

# Définir le compilateur explicitement pour Windows MSYS2
if (WIN32)
    set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe")
endif()

# Ajout d’options de compilation pour GCC
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# === GTK3 (sur Windows avec MSYS2 ) ===
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})


# === Client ===
file(GLOB CLIENT_SOURCES "client/src/*.c")
add_executable(client ${CLIENT_SOURCES})
include_directories(client/include)

target_link_libraries(client PRIVATE ws2_32 ${CJSON_LIBRARIES} ${GTK3_LIBRARIES})

# === Server ===
# file(GLOB SERVER_SOURCES "server/src/*.c")
# add_executable(server ${SERVER_SOURCES})
# target_link_libraries(server PRIVATE ws2_32)