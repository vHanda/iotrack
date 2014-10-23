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


add_executable(iotrack iotrack.cpp ptrace.cpp)
target_link_libraries(iotrack unwind-ptrace unwind-x86_64)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/iotrack DESTINATION ${BIN_INSTALL_DIR})
