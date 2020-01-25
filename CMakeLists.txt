################################################################################

cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)

################################################################################

project (nzmqt)

################################################################################

include(GenerateExportHeader)

################################################################################

set(NZMQT_VERSION_MAJOR 3)
set(NZMQT_VERSION_MINOR 5)
set(NZMQT_VERSION_PATCH 0)
set(NZMQT_VERSION_STRING ${NZMQT_VERSION_MAJOR}.${NZMQT_VERSION_MINOR}.${NZMQT_VERSION_PATCH})

################################################################################

option(ENABLE_TESTS "Build nzmqt test code" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

################################################################################

set(OUTPUT_BINDIR ${PROJECT_BINARY_DIR}/bin)
set(OUTPUT_LIBDIR ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIBDIR})
endif()

################################################################################

set(INSTALL_RUNTIME_DIR "bin")
if(WIN32)
    set(INSTALL_LIBRARY_DIR "bin")
else()
    set(INSTALL_LIBRARY_DIR "lib")
endif()
set(INSTALL_ARCHIVE_DIR "lib")
set(INSTALL_INCLUDE_DIR "include")
set(INSTALL_DATA_DIR    "share")

################################################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

################################################################################

find_package(Qt5Core REQUIRED)
find_package(cppzmq REQUIRED)

################################################################################

add_subdirectory(src)

################################################################################
