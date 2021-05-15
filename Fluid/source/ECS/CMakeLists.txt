
set(add_sources
	"ECS.h"
	"ECS.cpp")



list(TRANSFORM add_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
target_sources(${PROJECT_NAME} PRIVATE ${add_sources})
