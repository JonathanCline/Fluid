set(lib_subs
	"Art"
	"Component"
	"ECS"
	"Lua"
	"Utility"
	"Window")

macro(ADD_SUBDIRS_LIST subList)
	foreach(lfe IN LISTS ${subList})
		add_subdirectory(${lfe})
	endforeach()
endmacro(ADD_SUBDIRS_LIST)

ADD_SUBDIRS_LIST(lib_subs)


set(source_list
	"FluidState.h")

ADD_SOURCES_LIST(${PROJECT_NAME} source_list)
