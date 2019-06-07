set(CMAKE_INSTALL_PREFIX /usr/local)
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")

# Install desktop entry
install(FILES resources/FAConstructor.desktop PERMISSIONS WORLD_READ WORLD_EXECUTE DESTINATION /usr/share/applications )
# Install icon
install(DIRECTORY DESTINATION share/icons/FAConstructor)
install(FILES resources/FAConstructorIcon.png PERMISSIONS WORLD_READ WORLD_EXECUTE DESTINATION share/icons/FAConstructor)
# Program
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# autogenerate dependency information
set(CPACK_DEBIAN_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

set(CPACK_GENERATOR "DEB")
include(InstallRequiredSystemLibraries)
include(CPack)
