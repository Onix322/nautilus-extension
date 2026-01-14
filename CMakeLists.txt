
cmake_minimum_required(VERSION 3.16)
project(nautilus_ext LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
pkg_check_modules(NAUTILUS REQUIRED libnautilus-extension-4)

add_library(nautilus_ext SHARED main.cpp)

target_include_directories(nautilus_ext PRIVATE
    ${GTK4_INCLUDE_DIRS}
    ${NAUTILUS_INCLUDE_DIRS}
)

target_compile_options(nautilus_ext PRIVATE ${GTK4_CFLAGS_OTHER} ${NAUTILUS_CFLAGS_OTHER})
target_link_libraries(nautilus_ext PRIVATE ${GTK4_LIBRARIES} ${NAUTILUS_LIBRARIES})

