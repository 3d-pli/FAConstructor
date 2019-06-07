PROJECT(FAC CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE )
endif()

if(APPLE AND EXISTS /usr/local/opt/)
	# Homebrew installs Qt5 (up to at least 5.9.1) in
	# /usr/local/qt5, ensure it can be found by CMake since
	# it is not in the default /usr/local prefix.
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt")
	list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/boost")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	find_package( Qt5Widgets QUIET CONFIG REQUIRED )
	find_package( Qt5OpenGL QUIET CONFIG REQUIRED )
	set(LIBS Qt5::Widgets Qt5::WidgetsPrivate Qt5::OpenGL Qt5::OpenGLPrivate ${HDF5_CXX_LIBRARIES})
	set(CMAKE_CXX_FLAGS "/bigobj")
else()
	find_package( Qt5Widgets REQUIRED )
	find_package( Qt5OpenGL REQUIRED )
	set(LIBS Qt5::Widgets Qt5::OpenGL ${OpenGL_LIBRARIES} ${QT_LIBRARIES})
endif()

find_package( Boost QUIET COMPONENTS math_tr1 )

if(NOT Boost_FOUND OR Boost_VERSION LESS 106500)
    add_definitions(-DWITHOUT_BOOST)   
else()
    set(LIBS ${LIBS} ${Boost_LIBRARIES})
    include_directories(${Boost_INCLUDE_DIRS})
endif()

option(USE_OpenMP "Use OpenMP" ON)
if(USE_OpenMP)
    find_package( OpenMP QUIET )
    if (OPENMP_FOUND)
       set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
       #set (LIBS ${LIBS} OpenMP::OpenMP_C )
       set (OPENMP_FLAGS_APPENDED TRUE CACHE INTERNAL "")
    endif(OPENMP_FOUND)
endif(USE_OpenMP)

###########################################################################################
####################################### DEFINITIONS  ######################################
###########################################################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR})

set(SOURCE_DIR ${TOP_DIR}/source)
set(HEADER_DIR ${TOP_DIR}/source)
include_directories(${HEADER_DIR})

set(SOURCE
    ${SOURCE_DIR}/fiber.cpp
    ${SOURCE_DIR}/fiberfunction.cpp
    ${SOURCE_DIR}/fiberpoint.cpp
    ${SOURCE_DIR}/fiberpool.cpp
    ${SOURCE_DIR}/helpers.cpp
    ${SOURCE_DIR}/plugindata.cpp
)

set(HEADER
    ${HEADER_DIR}/fiber.h
    ${HEADER_DIR}/fiberfunction.h
    ${HEADER_DIR}/fiberpoint.h
    ${HEADER_DIR}/fiberpool.h
    ${HEADER_DIR}/helpers.h
    ${HEADER_DIR}/plugindata.h
    ${HEADER_DIR}/plugininterface.h
    ${HEADER_DIR}/exprtk.hpp
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(FAC STATIC ${SOURCE} ${HEADER})
else()
	add_library(FAC SHARED ${SOURCE} ${HEADER})
endif()

qt5_use_modules(FAC Core Widgets OpenGL)
target_link_libraries(FAC ${LIBS})

###########################################################################################
##########################################  TEST  #########################################
###########################################################################################
include(CTest)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(TARGETS FAC LIBRARY CONFIGURATIONS Release PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ DESTINATION lib)
    install(DIRECTORY DESTINATION include/FAC)
    install(FILES ${HEADER} DESTINATION include/FAC PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
endif()
