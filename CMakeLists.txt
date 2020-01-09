cmake_minimum_required(VERSION 3.1)

#set project name and language
project(vf-qml LANGUAGES CXX)

#Automoc and RCC ON. Needed in QT projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#C++ 11 Flag
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Set project Version
set(VfQml_VERSION_MAJOR "0")
set(VfQml_VERSION_MINOR "0")
set(VfQml_VERSION_PATCH "1")

set(PROJECT_VERSION "${VfQml_VERSION_MAJOR}.${VfQml_VERSION_MINOR}.${VfQml_VERSION_PATCH}")

#Adding cmake functions like feature_summary()
include(FeatureSummary)
include(GNUInstallDirs)

#Find dependecies
find_package(Qt5 COMPONENTS Core Qml CONFIG REQUIRED)
find_package(VfHelpers REQUIRED)
find_package(VfEvent REQUIRED)
find_package(VfComponent REQUIRED)
find_package(VfNet2 REQUIRED)
find_package(VfProtobuf REQUIRED)

#sum up project Files 
file(GLOB SOURCES 
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/*.h
    )

file(GLOB RESOURCES 
    ${PROJECT_SOURCE_DIR}/*.qrc
    )

#create library 
add_library(VfQml SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )



#link dependencies
target_link_libraries(VfQml 
    PRIVATE
    Qt5::Core
    Qt5::Qml
    VeinMeta::VfHelpers
    VeinMeta::VfEvent
    VeinMeta::VfComponent 
    VeinMeta::VfNet2
    VeinMeta::VfProtobuf
    )

# announce headers - target perspective
target_include_directories(VfQml
    PUBLIC
    $<INSTALL_INTERFACE:include/VfQml>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

    )

#set target Version
set_target_properties(VfQml PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(VfQml PROPERTIES SOVERSION ${VfQml_VERSION_MAJOR})

#generate export header
include(GenerateExportHeader)
generate_export_header(VfQml)

#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vfqml_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/VfQml
    )



# install library
install(TARGETS VfQml
    EXPORT VfQmlExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    VfQmlConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(VfQmlConfig.cmake.in VfQmlConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/VfQmlConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/VfQmlConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VfQml
    )

# install targets cmake-files
install(EXPORT VfQmlExport
    FILE VfQmlTargets.cmake
    NAMESPACE VeinMeta::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VfQml
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/VfQml
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



