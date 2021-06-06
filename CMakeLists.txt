cmake_minimum_required(VERSION 3.18.0)
project(Avo VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

#--------------------------------------------
# Library target.

add_library(avo STATIC)
add_library(${PROJECT_NAME}::avo ALIAS avo)

#--------------------------------------------
# Set up library target options.

target_compile_features(avo PUBLIC cxx_std_20)
set_target_properties(avo PROPERTIES CXX_EXTENSIONS off)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	target_compile_options(avo PUBLIC
		/experimental:external
		/external:anglebrackets
		/external:W0 
		/WX /Wall
	)
else ()
	target_compile_options(avo PUBLIC
		-Werror -Wall -Wpedantic -Wextra 
		-Wimplicit-fallthrough=5 
		-Wduplicated-branches -Wduplicated-cond 
		-Wcast-qual -Wconversion
		
		-Wno-parentheses
		-Wno-missing-field-initializers

		-fconcepts-diagnostics-depth=2
	)
endif ()

#--------------------------------------------

target_include_directories(avo PUBLIC
	# When using the library from the install tree, relative paths can be used.
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

add_subdirectory(source)

#--------------------------------------------
# Add cross-platform external libraries.

include(FetchContent)

FetchContent_Declare(
	fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG 7.1.3
)
FetchContent_MakeAvailable(fmt)

target_link_libraries(avo PUBLIC fmt::fmt)
# target_precompile_headers(avo PRIVATE <fmt/format.h> <fmt/ranges.h>)

#--------------------------------------------
# Set up testing.

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
	if (BUILD_TESTING)
		add_subdirectory(tests)
	endif ()
endif ()

add_subdirectory(examples)

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
	TARGETS avo fmt
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
