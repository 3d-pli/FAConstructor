###########################################################################################
##########################################  TESTS  ########################################
###########################################################################################
find_package( Qt5Test REQUIRED )
set(LIBS FACGUI Qt5::Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ElapsedTimer
add_executable(elapsedtimer ElapsedTimerTest.cpp)
target_link_libraries(elapsedtimer ${LIBS})
add_test(elapsedtimer elapsedtimer)

# FiberEditWidget
add_executable(fibereditwidget FiberEditWidgetTest.cpp)
target_link_libraries(fibereditwidget ${LIBS})
add_test(fibereditwidget fibereditwidget)

# FileIO
add_executable(fileio FileIOTest.cpp)
target_link_libraries(fileio ${LIBS})
add_test(fileio fileio)

# OptionsWidget
add_executable(optionswidget OptionsWidgetTest.cpp)
target_link_libraries(optionswidget ${LIBS})
add_test(optionswidget optionswidget)
