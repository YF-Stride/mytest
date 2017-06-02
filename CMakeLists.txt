project(test_para)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS -std=c99)

set(SOURCE_FILES  test_plane_para.c)

add_executable(test_para ${SOURCE_FILES})

target_link_libraries(test_para lightnvm)
