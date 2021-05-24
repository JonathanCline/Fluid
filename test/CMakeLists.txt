
add_executable(fluidtest "test.cpp")
target_link_libraries(fluidtest PUBLIC Fluid PRIVATE SAELib liblua)
set_target_properties(fluidtest PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)


target_compile_definitions(fluidtest PRIVATE PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")

