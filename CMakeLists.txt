cmake_minimum_required(VERSION 3.1)

project(vf-qml LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(VfQml_VERSION_MAJOR "0")
set(VfQml_VERSION_MINOR "0")
set(VfQml_VERSION_PATCH "1")
set(PROJECT_VERSION "${VfQml_VERSION_MAJOR}.${VfQml_VERSION_MINOR}.${VfQml_VERSION_PATCH}")

include(FeatureSummary)
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

find_package(Qt5 COMPONENTS Core Qml CONFIG REQUIRED)
find_package(VeinFramework REQUIRED)
find_package(VfNet2 REQUIRED)

file(GLOB SOURCES 
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/*.h
    )

add_library(VfQml SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(VfQml 
    PRIVATE
    Qt5::Core
    Qt5::Qml
    Zera::VfHelpers
    Zera::VfEvent
    Zera::VfComponent 
    VeinMeta::VfNet2
    )

target_include_directories(VfQml
    PUBLIC
    $<INSTALL_INTERFACE:include/VfQml>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(VfQml PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(VfQml PROPERTIES SOVERSION ${VfQml_VERSION_MAJOR})

generate_export_header(VfQml)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vfqml_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/VfQml
    )

install(TARGETS VfQml
    EXPORT VfQmlExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    VfQmlConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

configure_file(VfQmlConfig.cmake.in VfQmlConfig.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/VfQmlConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/VfQmlConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VfQml
    )

install(EXPORT VfQmlExport
    FILE VfQmlTargets.cmake
    NAMESPACE VeinMeta::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VfQml
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/VfQml
    )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
