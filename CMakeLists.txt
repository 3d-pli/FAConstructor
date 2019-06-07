PROJECT(FAConstructor CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

SET(FAConstructor_MAJOR_VERSION 1)
SET(FAConstructor_MINOR_VERSION 7)
SET(FAConstructor_PATCH_VERSION 0)

# CMake Policies
if(POLICY CMP0020)
    cmake_policy( SET CMP0020 NEW )
endif()

if(POLICY CMP0072)
    set(OpenGL_GL_PREFERENCE "GLVND")
    cmake_policy( SET CMP0072 NEW )
endif()

if(POLICY CMP0080)
    cmake_policy( SET CMP0080 OLD )
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

###########################################################################################
###################################### OTHER PROJECTS #####################################
###########################################################################################

add_subdirectory(FAC)
include_directories(FAC/source)
add_subdirectory(FACGUI)
include_directories(FACGUI/source)

###########################################################################################
######################################### PLUG INS ########################################
###########################################################################################

add_subdirectory(plugins/pseudofomplugin)

###########################################################################################
####################################### SOURCE FILES ######################################
###########################################################################################

configure_file(
	${CMAKE_CURRENT_LIST_DIR}/resources/FAConstructor.desktop.in
	${CMAKE_CURRENT_LIST_DIR}/resources/FAConstructor.desktop
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set( ICON_NAME "FAConstructorIcon.icns" )
    set( ICON_PATH "resources/${ICON_NAME}" )
    set_source_files_properties( ${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE main.cpp ${ICON_PATH})
    set_target_properties( ${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE ${ICON_NAME} )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_executable(${PROJECT_NAME} main.cpp ${RESOURCES_RCC})
else()
    add_executable(${PROJECT_NAME} main.cpp)
endif()

target_link_libraries(
    ${PROJECT_NAME}
    ${LIBS}
    FACGUI
)

###########################################################################################
########################################## INSTALL ########################################
###########################################################################################

# Installation
include("${CMAKE_CURRENT_LIST_DIR}/cmake/CPackConfig.cmake")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/InstallLinux.cmake")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/InstallMacOS.cmake")
endif()
