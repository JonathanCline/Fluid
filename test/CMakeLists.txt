
add_executable(fluidtest "test.cpp")
target_link_libraries(fluidtest PUBLIC Fluid)
set_target_properties(fluidtest PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED True)
