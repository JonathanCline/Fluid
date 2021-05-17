cmake_minimum_required(VERSION 3.8)

project(Fluid)



set(lib_include_public
	"include")

set(lib_include_private
	"source"
	"source/Lua")

set(lib_link_public
	)

set(lib_link_private
	glfw
	glad
	liblua

	JCLib
	jclua
	
	SAELib)

set(lib_cxx 20)

set(lib_namespace fluid)


add_library(${PROJECT_NAME} STATIC
	"include/${PROJECT_NAME}.h"
	"source/${PROJECT_NAME}.cpp")


foreach(lib_dependency ${lib_link_private} ${lib_link_public})
	if (NOT TARGET ${lib_dependency})
		message(FATAL_ERROR "missing dependency '${lib_dependency}' for project '${PROJECT_NAME}'")
	endif()
endforeach()

	
target_include_directories(${PROJECT_NAME} PUBLIC ${lib_include_public} PRIVATE ${lib_include_private})
target_link_libraries(${PROJECT_NAME} PUBLIC ${lib_link_public} PRIVATE ${lib_link_private})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${lib_cxx} CXX_STANDARD_REQUIRED True)

target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_NAMESPACE=${lib_namespace})

add_subdirectory("source")