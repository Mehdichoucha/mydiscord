cmake_minimum_required(VERSION 3.10)
project(mon_app C)

set(CMAKE_C_STANDARD 11)

# ==== GTK4 ====
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# ==== PostgreSQL / libpq via MSYS2 ====
# Installation nécessaire via : pacman -S mingw-w64-x86_64-postgresql
set(PostgreSQL_INCLUDE_DIRS "C:/msys64/mingw64/include")
set(PostgreSQL_LIBRARY_DIRS "C:/msys64/mingw64/lib")
set(PostgreSQL_LIBRARIES pq)

# === Client ===
file(GLOB CLIENT_SOURCES "client/src/*.c")
add_executable(client ${CLIENT_SOURCES})

# Include directories
target_include_directories(client PRIVATE
    ${GTK4_INCLUDE_DIRS}
    ${PostgreSQL_INCLUDE_DIRS}
    client/include
)

# Link directories
link_directories(${GTK4_LIBRARY_DIRS} ${PostgreSQL_LIBRARY_DIRS})

# Link libraries
target_link_libraries(client PRIVATE ${GTK4_LIBRARIES} ${PostgreSQL_LIBRARIES})

# Flags
add_definitions(${GTK4_CFLAGS_OTHER})
