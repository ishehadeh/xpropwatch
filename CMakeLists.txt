CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
project(xpropwatch)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

find_package(XCB)

include_directories("${CMAKE_SOURCE_DIR}/src" ${XCB_INCLUDE_DIR})
link_libraries(${XCB_LIBRARY_DIR})

add_executable(xpropwatch "${CMAKE_SOURCE_DIR}/src/core.cpp" "${CMAKE_SOURCE_DIR}/src/xpropwatch.cpp")
target_link_libraries(xpropwatch xcb)
