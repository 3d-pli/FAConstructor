cmake_minimum_required(VERSION 3.5)
PROJECT(PseudoFOMPlugin CXX C)

find_package(OpenCV QUIET REQUIRED)
find_package(Qt5Widgets QUIET REQUIRED)
set(LIBS Qt5::Widgets ${OpenCV_LIBS})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR})

add_library(${PROJECT_NAME} SHARED PseudoFOMPlugin.cpp)
qt5_use_modules(${PROJECT_NAME} Core Widgets)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_subdirectory(../../FAC build/)
    include_directories(../../FAC/source)
else()
    add_dependencies(${PROJECT_NAME} FAC)
endif()
set(LIBS ${LIBS} FAC)

target_link_libraries(${PROJECT_NAME} ${LIBS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(DIRECTORY DESTINATION lib/FAC_Plugin)
    install(TARGETS ${PROJECT_NAME} LIBRARY CONFIGURATIONS Release PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ DESTINATION lib/FAC_Plugin)
endif()
