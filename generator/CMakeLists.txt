add_executable(iotrack iotrack.cpp ptrace.cpp)
target_link_libraries(iotrack
    unwind-ptrace unwind-x86_64
    Qt5::Core
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/iotrack DESTINATION ${BIN_INSTALL_DIR})
