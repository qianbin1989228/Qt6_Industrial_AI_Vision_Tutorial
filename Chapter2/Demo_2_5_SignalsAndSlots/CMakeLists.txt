cmake_minimum_required(VERSION 3.19)
project(Demo_2_5_SignalsAndSlots LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(Demo_2_5_SignalsAndSlots
    main.cpp
    worker.h worker.cpp
    manager.h manager.cpp
)

target_link_libraries(Demo_2_5_SignalsAndSlots
    PRIVATE
        Qt::Core
)
target_link_libraries(Demo_2_5_SignalsAndSlots PRIVATE Qt6::Core)
target_link_libraries(Demo_2_5_SignalsAndSlots PRIVATE Qt6::Core)

include(GNUInstallDirs)

install(TARGETS Demo_2_5_SignalsAndSlots
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
