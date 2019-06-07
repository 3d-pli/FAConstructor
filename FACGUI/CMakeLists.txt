PROJECT(FACGUI CXX C)
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

SET(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
     ${CMAKE_MODULE_PATH})

###########################################################################################
######################################### PACKAGES ########################################
###########################################################################################

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

find_package ( OpenGL REQUIRED )

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	find_package( HDF5 REQUIRED COMPONENTS C CXX HL )
	find_package( ZLIB REQUIRED )
	find_package( szip REQUIRED )
	set(LIBS ${HDF5_CXX_LIBRARIES} ${HDF5_LIBRARIES} ${HDF5_C_LIBRARIES} ${ZLIB_LIBRARIES} ${SZIP_LIBRARIES} ${OpenGL_LIBRARIES})
	include_directories(${HDF5_INCLUDE_DIR})
	set(CMAKE_CXX_FLAGS "/bigobj")
else()
	find_package( HDF5 REQUIRED COMPONENTS C CXX HL )
	set(LIBS ${OpenGL_LIBRARIES} ${HDF5_LIBRARIES} ${QT_LIBRARIES})
	include_directories(${HDF5_INCLUDE_DIR})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(COCOA_LIBRARY Cocoa)
    set(LIBS ${LIBS} ${COCOA_LIBRARY})
endif()

###########################################################################################
####################################### DEFINITIONS  ######################################
###########################################################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

###########################################################################################
####################################### SOURCE FILES ######################################
###########################################################################################

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SOURCE_DIR ${TOP_DIR}/source)
set(HEADER_DIR ${TOP_DIR}/source)
include_directories(${HEADER_DIR})
set(RESOURCE_DIR ${TOP_DIR}/resources)

set(SOURCE
    ${SOURCE_DIR}/elapsedtimer.cpp
    ${SOURCE_DIR}/mainwindow.cpp
    ${SOURCE_DIR}/render2d.cpp
    ${SOURCE_DIR}/render3d.cpp
    ${SOURCE_DIR}/renderbundle.cpp
    ${SOURCE_DIR}/renderdata.cpp
    ${SOURCE_DIR}/renderwidget.cpp
    ${SOURCE_DIR}/optionswidget.cpp
    ${SOURCE_DIR}/pluginloader.cpp
    ${SOURCE_DIR}/fibereditwidget.cpp
    ${SOURCE_DIR}/settingswidget.cpp
    ${SOURCE_DIR}/settings.cpp
    ${SOURCE_DIR}/fileio.cpp
    ${SOURCE_DIR}/transformation3d.cpp
    ${SOURCE_DIR}/vertex.cpp
)

set(HEADER
    ${HEADER_DIR}/fileio.h
    ${HEADER_DIR}/elapsedtimer.h
    ${HEADER_DIR}/mainwindow.h
    ${HEADER_DIR}/render2d.h
    ${HEADER_DIR}/render3d.h
    ${HEADER_DIR}/renderbundle.h
    ${HEADER_DIR}/renderdata.h
    ${HEADER_DIR}/renderwidget.h
    ${HEADER_DIR}/optionswidget.h
    ${HEADER_DIR}/pluginloader.h
    ${HEADER_DIR}/fibereditwidget.h
    ${HEADER_DIR}/settingswidget.h
    ${HEADER_DIR}/settings.h
    ${HEADER_DIR}/version.h
    ${HEADER_DIR}/widgetdef.h
    ${HEADER_DIR}/transformation3d.h
    ${HEADER_DIR}/vertex.h
)

configure_file(
    ${HEADER_DIR}/version.h.in
    ${HEADER_DIR}/version.h)

set(RESOURCE
    ${RESOURCE_DIR}/resource.qrc)
QT5_ADD_RESOURCES(RESOURCES_RCC ${RESOURCE})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(FACGUI STATIC ${SOURCE} ${HEADER} ${RESOURCES_RCC})
else()
	add_library(FACGUI SHARED ${SOURCE} ${HEADER} ${RESOURCES_RCC})
endif()
target_include_directories(FACGUI PUBLIC ${HEADER_DIR} ${HDF5_INCLUDE_DIR})
qt5_use_modules(FACGUI Core Widgets OpenGL)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    # I am top-level project.
    add_subdirectory(../FAC build/)
    include_directories(../FAC/source)
else()
    # I am called from other project with add_subdirectory().
    add_dependencies(FACGUI FAC)
endif()

target_link_libraries(FACGUI ${LIBS} FAC)

###########################################################################################
##########################################  TEST  #########################################
###########################################################################################
include(CTest)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	install(TARGETS FACGUI CONFIGURATIONS Release LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ DESTINATION lib)
endif()
