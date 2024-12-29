cmake_minimum_required(VERSION 3.15...3.31)
project(Avo VERSION 0.1.0 LANGUAGES CXX)

#--------------------------------------------
# Library target.

add_library(avo STATIC)
add_library(Avo::avo ALIAS avo)

#--------------------------------------------
# Set up library target options.

target_compile_features(avo PUBLIC cxx_std_23)
set_target_properties(avo PROPERTIES CXX_EXTENSIONS off)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	add_compile_options(/utf-8)
endif ()

#--------------------------------------------

target_include_directories(avo PUBLIC
	# When using the library from the install tree, relative paths can be used.
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

add_subdirectory(source)

#--------------------------------------------
# Set up testing.

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
	if (BUILD_TESTING)
		add_subdirectory(tests)
	endif ()

	add_subdirectory(examples)
endif ()

#--------------------------------------------
# Set up installation.

install(
	TARGETS avo
	EXPORT AvoTargets
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)

install(DIRECTORY include DESTINATION .)

install(
	EXPORT AvoTargets
	NAMESPACE Avo::
	DESTINATION lib/cmake/Avo
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  AvoConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
)

configure_file(cmake/AvoConfig.cmake.in AvoConfig.cmake @ONLY)

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/AvoConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/AvoConfigVersion.cmake"
	DESTINATION lib/cmake/Avo
)

