
# Add sources

target_sources(avo PRIVATE window.cpp unicode.cpp)

#--------------------------------------------

# Link to platform-dependent external libraries.

if (WIN32)
else ()
	find_package(X11 REQUIRED)
	target_link_libraries(avo PRIVATE X11)
	
	find_package(OpenGL REQUIRED)
	target_link_libraries(avo PRIVATE OpenGL::OpenGL OpenGL::GLX)
	
	find_package(Threads REQUIRED)
	target_link_libraries(avo PRIVATE Threads::Threads)

	find_package(Iconv REQUIRED)
	if (NOT Iconv_IS_BUILT_IN)
		target_link_libraries(avo PRIVATE Iconv::Iconv)
	endif ()
endif ()
