cmake_minimum_required(VERSION 3.16)
project(Image_Display VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)


# Find OpenCV package
find_package(OpenCV REQUIRED)


add_executable(Image_Display WIN32 MACOSX_BUNDLE
    image_base.cpp image_base.h
    main.cpp
)
target_link_libraries(Image_Display PRIVATE
    Qt::Core
    Qt::Gui

    ${OpenCV_LIBS}  # Link OpenCV libraries
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(Image_Display PRIVATE
        Qt::Widgets
    )
endif()

install(TARGETS Image_Display
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
