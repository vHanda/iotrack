add_executable(iotrack_visualize functionsmodel.cpp main.cpp)
target_link_libraries(iotrack_visualize
    Qt5::Widgets
)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/iotrack_visualize DESTINATION ${BIN_INSTALL_DIR})
