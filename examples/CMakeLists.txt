add_executable(scene_demo scene_demo.cpp)
target_link_libraries(scene_demo PRIVATE wlrootspp)

add_executable(tinywl tinywl.cpp)
target_link_libraries(tinywl PRIVATE wlrootspp)
