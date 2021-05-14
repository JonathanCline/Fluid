cmake_minimum_required(VERSION 3.8)

project(Fluid)

add_library(${PROJECT_NAME} STATIC
	"include/${PROJECT_NAME}.h"
	"source/${PROJECT_NAME}.cpp")
	
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "source")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)
