cmake_minimum_required(VERSION 3.8)


if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/local")
	add_subdirectory("local")
endif()






macro(ADD_SOURCES_LIST toTarget sourceList)
	list(TRANSFORM ${sourceList} PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
	set(lfiles )
	foreach(lfe IN LISTS ${sourceList})
		list(APPEND lfiles ${lfe})
	endforeach()
	target_sources(${toTarget} PRIVATE ${lfiles})
endmacro(ADD_SOURCES_LIST)

macro(ADD_SUBDIRS_LIST subList)
	foreach(lfe IN LISTS ${subList})
		add_subdirectory(${lfe})
	endforeach()
endmacro(ADD_SUBDIRS_LIST)


add_subdirectory("Fluid")
add_subdirectory("test")

