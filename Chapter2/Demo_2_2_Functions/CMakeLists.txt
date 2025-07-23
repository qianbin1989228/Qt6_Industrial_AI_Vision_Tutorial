cmake_minimum_required(VERSION 3.19)
project(Demo_2_2_Functions LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(Demo_2_2_Functions
    main.cpp
)

target_link_libraries(Demo_2_2_Functions
    PRIVATE
        Qt::Core
)

include(GNUInstallDirs)

install(TARGETS Demo_2_2_Functions
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
