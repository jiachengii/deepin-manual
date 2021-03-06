
cmake_minimum_required(VERSION 3.7)

if (NOT DEFINED VERSION)
    set(VERSION 1.2.2)
endif ()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#if (CMAKE_BUILD_TYPE MATCHES Debug)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Werror -Wextra")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror -Wextra -fno-rtti -fno-exceptions")
#  add_definitions("-DQT_MESSAGELOGCONTEXT")
#else()
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2 -Wl,-O1 -Wl,--gc-sections")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -Wl,-O1 -Wl,--gc-sections")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
#endif()

find_package(PkgConfig REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(Qt5LinguistTools)
find_package(Qt5Sql REQUIRED)
find_package(Qt5WebChannel REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)

find_package(Qt5Test REQUIRED)

pkg_search_module(DtkWidget REQUIRED dtkwidget)
pkg_search_module(DtkGui REQUIRED dtkgui)
pkg_search_module(DtkCore REQUIRED dtkcore)

include_directories(${DtkWidget_INCLUDE_DIRS})
include_directories(${DtkGui_INCLUDE_DIRS})
include_directories(${DtkCore_INCLUDE_DIRS})

set(LINK_LIBS
    Qt5::Core
    Qt5::DBus
    Qt5::Sql
    Qt5::Widgets
    Qt5::X11Extras
    Qt5::WebChannel
    Qt5::WebEngineWidgets
    Qt5::Test

    ${DtkWidget_LDFLAGS}
    ${DtkGui_LDFLAGS}
    ${DtkCore_LDFLAGS}

    -lX11)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DDMAN_SEARCH_DB="${CMAKE_SOURCE_DIR}/manual-assets")
  add_definitions(-DDMAN_WEB_DIR="${CMAKE_CURRENT_SOURCE_DIR}/web_dist")
  add_definitions(-DDMAN_MANUAL_DIR="${CMAKE_SOURCE_DIR}/manual-assets")
else()
  add_definitions(-DDMAN_SEARCH_DB="${CMAKE_INSTALL_PREFIX}/share/deepin-manual/manual-assets")
  add_definitions(-DDMAN_WEB_DIR="${CMAKE_INSTALL_PREFIX}/share/deepin-manual/web_dist")
  add_definitions(-DDMAN_MANUAL_DIR="${CMAKE_INSTALL_PREFIX}/share/deepin-manual/manual-assets")
endif ()

#configure_file(../src/environments.h.in environments.h @ONLY)

# These macros ared used at compile time only.
add_definitions(-DDMAN_SEARCH_ORIG_DB="${CMAKE_CURRENT_BINARY_DIR}/search.db")
add_definitions(-DDMAN_ORIG_MANUAL_DIR="${CMAKE_SOURCE_DIR}/manual-assets")
add_definitions(-DDMAN_SEARCH_WORK_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../src/web")
add_definitions(-DDMAN_SEARCH_CREATE_DB_PATH="${CMAKE_CURRENT_BINARY_DIR}/dbPath")
# This is for icon path on db
add_definitions(-DDMAN_INSTALL_DB_PATH="${CMAKE_INSTALL_PREFIX}/share/deepin-manual/manual-assets")

include_directories(googletest)
include_directories(googletest/googlemock/include)
include_directories(googletest/googletest/include)

#GTest & GMock
add_subdirectory(googletest)

include_directories(../src)

set(PROJECT_NAME_TEST
    ${PROJECT_NAME}_test)

# 定义 source 文件
FILE (GLOB allTestSource
    src/base/*.cpp
    src/controller/*.cpp
    src/dbus/*.cpp
    src/view/*.cpp
    src/view/widget/*.cpp
    src/test_main.cpp
    )

#Include all app source files
FILE (GLOB dmanCppFiles
    ../src/base/*.cpp
    ../src/controller/*.cpp
    ../src/dbus/*.cpp
    ../src/dbus/dbusvariant/*.cpp
#    ../src/resources/*
    ../src/view/*.cpp
    ../src/view/widget/*.cpp)

FILE (GLOB dmanHeaderFiles
    ../src/base/*.h
    ../src/controller/*.h
    ../src/dbus/*.h
    ../src/dbus/dbusvariant/*.h
#    ../src/resources/*.h
    ../src/view/*.h
    ../src/widget/*.h)

FILE (GLOB dmanResourcesFiles
    ../src/resources/themes/images.cpp
    ../src/resources/themes/images.h
    ../src/resources/themes/themes.qrc
    )

# 生成测试可执行程序
add_executable(${PROJECT_NAME_TEST}
    ${dmanCppFiles}
    ${dmanHeaderFiles}
    ${dmanResourcesFiles}
    ${allTestSource}
)

target_include_directories(${PROJECT_NAME_TEST} PUBLIC ${Qt5Widgets_LIBRARIES}
                                                       ${Qt5DBus_LIBRARIES}
                                                       ${Qt5TestLib_LIBRARIES}
                                                       ${Qt5Widgets_PRIVATE_INCLUDE_DIRS}
                                                       ${Qt5Core_PRIVATE_INCLUDE_DIRS}
                                                       ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
                                                       ${DtkWidget_INCLUDE_DIRS}
                                                       ${DtkCore_INCLUDE_DIRS}
                                                       ${DtkGui_INCLUDE_DIRS}
                                                       ${PROJECT_BINARY_DIR})
# 生成测试可执行程序的依赖库
target_link_libraries(${PROJECT_NAME_TEST} PUBLIC ${LINK_LIBS} gmock gmock_main gtest gtest_main)

#------------------------------ 创建'make test'指令---------------------------------------
add_custom_target(test
    #执行mkdir -p 创建 coverageResult
    COMMAND pwd
    COMMAND mkdir -p coverageResult
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_command(TARGET test
    COMMAND echo " =================== CREAT LCOV REPROT BEGIN ==================== "

    #1.清理原先的gcov信息
    COMMAND lcov --directory ./CMakeFiles/${PROJECT_NAME_TEST}.dir --zerocounters
    COMMAND lcov --directory ./CMakeFiles/${PROJECT_NAME}.dir --zerocounters
    COMMAND ./dman_qtest/${PROJECT_NAME_TEST}

    #2.收集gcov信息到.info文件中
    COMMAND lcov --directory . --capture --output-file ./coverageResult/${PROJECT_NAME}_Coverage.info

    #3.过滤一些我们不感兴趣的文件的覆盖率信息
    COMMAND echo " =================== do filter begin ==================== "
    COMMAND lcov --remove ./coverageResult/${PROJECT_NAME}_Coverage.info
    '*/${PROJECT_NAME_TEST}_autogen/*' '*/${PROJECT_NAME}_autogen/*' '*/usr/include/*' '*/dman_qtest/*' '*/googletest/*' '*/src/*autogen*/*' '*/app/*'
    -o ./coverageResult/${PROJECT_NAME}_Coverage.info
    COMMAND echo " =================== do filter end ==================== "

    #3.将.info信息生成报告到reprot文件夹中
    COMMAND genhtml -o ./coverageResult/report ./coverageResult/${PROJECT_NAME}_Coverage.info

    COMMAND echo " =================== CREAT LCOV REPROT END ==================== "

    COMMAND echo " Coverage files have been output to ${CMAKE_BINARY_DIR}/coverageResult/report "

    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

#'make test'命令依赖与我们的测试程序
add_dependencies(test ${PROJECT_NAME_TEST})

# 设置添加gocv相关信息的输出
set(CMAKE_CXX_FLAGS "-g -fprofile-arcs -ftest-coverage")


