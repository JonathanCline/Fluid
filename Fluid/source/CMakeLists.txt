set(lib_subs
	"ECS"
	"Component"
	"Lua"
	"Utility"
	"Window")

macro(ADD_SUBDIRS_LIST subList)
	foreach(lfe IN LISTS ${subList})
		add_subdirectory(${lfe})
	endforeach()
endmacro(ADD_SUBDIRS_LIST)

ADD_SUBDIRS_LIST(lib_subs)

