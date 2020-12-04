cmake_minimum_required(VERSION 3.18.0)
project(AvoGUI VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

#--------------------------------------------
# Library target.

add_library(avogui STATIC include/AvoGUI.hpp source/AvoGUI.cpp)
add_library(${PROJECT_NAME}::avogui ALIAS avogui)

target_compile_features(avogui PUBLIC cxx_std_20)
set_target_properties(avogui PROPERTIES CXX_EXTENSIONS off)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	target_compile_options(avogui PUBLIC
		/experimental:external
		/external:anglebrackets
		/external:W0 
		/WX /Wall
	)
else ()
	target_compile_options(avogui PUBLIC
		-Werror -Wall -Wpedantic -Wextra 
		-Wimplicit-fallthrough=5 
		-Wduplicated-branches -Wduplicated-cond 
		-Wcast-qual -Wconversion
		
		-Wno-parentheses
		-Wno-missing-field-initializers
	)
endif ()

target_include_directories(avogui PUBLIC
	# When using the library from the install tree, relative paths can be used.
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (WIN32)
    target_link_libraries(avogui PRIVATE Shlwapi Dwmapi d2d1 d3d11 dxguid dwrite windowscodecs)
else ()

endif ()

#--------------------------------------------
# Set up installation.

include(CMakePackageConfigHelpers)

set(CONFIG_VERSION_FILE_NAME ${PROJECT_NAME}ConfigVersion.cmake)

# Create a file that contains information about package versioning.
write_basic_package_version_file(
    ${CONFIG_VERSION_FILE_NAME}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# During installation, the version file will be installed.
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_VERSION_FILE_NAME}"
    DESTINATION lib/cmake/${PROJECT_NAME}) # Relative to the installation path.

set(TARGET_EXPORT_NAME ${PROJECT_NAME}Targets)

# Specifies the target(s) that will be installed, and where to install 
# the compiled library (relative to package installation path ${CMAKE_INSTALL_PREFIX}).
install(
    TARGETS avogui
    EXPORT ${TARGET_EXPORT_NAME}
    ARCHIVE DESTINATION lib
)

# During installation, a target configuration file will be exported to a *Targets.cmake file
# that is included by the *Config.cmake.in file which finds the dependencies of the library.
install(
    EXPORT ${TARGET_EXPORT_NAME}
	FILE ${TARGET_EXPORT_NAME}.cmake
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION lib/cmake/${PROJECT_NAME}
)

# This uses the *Config.cmake.in file to generate a *Config.cmake file with
# the variables passed through the PATH_VARS parameter inserted.
configure_package_config_file(
	cmake/${PROJECT_NAME}Config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
	INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
	PATH_VARS TARGET_EXPORT_NAME
)

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
	DESTINATION lib/cmake/${PROJECT_NAME}
)

install(DIRECTORY include DESTINATION .)
