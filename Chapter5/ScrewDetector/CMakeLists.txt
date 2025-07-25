cmake_minimum_required(VERSION 3.16)

project(ScrewDetector VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)


# --- 开始集成OpenCV ---
# 1. 设置OpenCV的根目录，请根据您的实际安装路径修改
set(OpenCV_DIR "D:/toolplace/opencv4.12.0/build")

# 2. 查找OpenCV包，Core和Imgproc是我们目前需要的核心和图像处理模块
find_package(OpenCV REQUIRED COMPONENTS core imgproc)

# 3. 包含OpenCV的头文件目录，以便#include指令能够找到它们
include_directories(${OpenCV_INCLUDE_DIRS})
# --- 结束集成OpenCV ---


qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appScrewDetector
    main.cpp
    logo.rc
)

qt_add_qml_module(appScrewDetector
    URI ScrewDetector
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES backend.h backend.cpp
        SOURCES imageprovider.h imageprovider.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appScrewDetector PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appScrewDetector
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appScrewDetector
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Core
    ${OpenCV_LIBS} # 链接OpenCV库
)
target_link_libraries(appScrewDetector PRIVATE Qt6::Core)

include(GNUInstallDirs)
install(TARGETS appScrewDetector
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_add_resources(appScrewDetector "app_resources"
    PREFIX "/"
    FILES
        icons/appicon.png
)
