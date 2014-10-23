cmake_minimum_required(VERSION 2.8)
project(iotrack)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)
include(ECMInstallIcons)
include(ECMAddTests)

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS Widgets)
#find_package(KF5 REQUIRED COMPONENTS CoreAdd)

add_subdirectory(generator)
add_subdirectory(visualizer)

