###########################################################################################
##########################################  TESTS  ########################################
###########################################################################################
find_package( Qt5Test REQUIRED )
set(LIBS ${LIBS} FAC Qt5::Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# FiberFunction
add_executable(fiberfunction FiberFunctionTest.cpp)
target_link_libraries(fiberfunction ${LIBS})
add_test(fiberfunction fiberfunction)

# FiberPoint
add_executable(fiberpoint FiberPointTest.cpp)
target_link_libraries(fiberpoint ${LIBS})
add_test(fiberpoint fiberpoint)

# FiberPool
add_executable(fiberpool FiberPoolTest.cpp)
target_link_libraries(fiberpool ${LIBS})
add_test(fiberpool fiberpool)

# Fiber
add_executable(fiber FiberTest.cpp)
target_link_libraries(fiber ${LIBS})
add_test(fiber fiber)

# Helpers
add_executable(helpers HelpersTest.cpp)
target_link_libraries(helpers ${LIBS})
add_test(helpers helpers)