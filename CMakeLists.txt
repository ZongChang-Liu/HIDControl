cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME "HIDControl")
project(HIDControl VERSION 0.2.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(ELAWIDGETTOOLS_PREFIX_DIR "${PROJECT_SOURCE_DIR}/3rdparty/ElaWidgetTools")
set(HIDAPI_PREFIX_DIR "${PROJECT_SOURCE_DIR}/3rdparty/hidapi")
set(CMAKE_PREFIX_PATH "${ELAWIDGETTOOLS_PREFIX_DIR}/lib/cmake" "${HIDAPI_PREFIX_DIR}/lib/cmake")

set(QT_MODULES
        Core
        Gui
        Widgets
)
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS ${QT_MODULES})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_MODULES})
message(STATUS "Qt version: ${Qt${QT_VERSION_MAJOR}_VERSION}")
find_package(ElaWidgetTools REQUIRED)
find_package(hidapi REQUIRED)
file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
qt5_add_big_resources(PROJECT_SOURCES resources.qrc ${ELAWIDGETTOOLS_PREFIX_DIR}/include/ElaWidgetTools.qrc)

include_directories(${PROJECT_NAME} "src")
add_executable(${PROJECT_NAME}
#        WIN32
        main.cpp ${HEADERS} ${SOURCES} ${PROJECT_SOURCES} logo.rc
        src/control/HIDDevice.cpp
        src/control/HIDDevice.h)
foreach (QT_MODULE ${QT_MODULES})
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::${QT_MODULE})
endforeach (QT_MODULE)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${ELAWIDGETTOOLS_INCLUDE_DIRS}
    ${HIDAPI_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PUBLIC ElaWidgetTools hidapi::hidapi)
target_link_directories(${PROJECT_NAME} PUBLIC ${ELAWIDGETTOOLS_LIBRARY_DIRS} ${HIDAPI_LIBRARY_DIRS})
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${ELAWIDGETTOOLS_PREFIX_DIR}/bin/ElaWidgetTools.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${HIDAPI_PREFIX_DIR}/bin/hidapi.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif ()
