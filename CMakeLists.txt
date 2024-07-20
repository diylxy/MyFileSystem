# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.5)

# 项目信息
project (simpleFS)

set(CMAKE_BUILD_TYPE Debug)

aux_source_directory(src DIR_SRCS)

include_directories (include)

add_executable(simpleFS ${DIR_SRCS} test/fs.c)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Building on Linux")
    add_library(SimpleFS_a STATIC ${DIR_SRCS})
    set_property(TARGET SimpleFS_a PROPERTY OUTPUT_NAME SimpleFS)
    add_library(SimpleFS_so SHARED ${DIR_SRCS})
    set_property(TARGET SimpleFS_so PROPERTY OUTPUT_NAME SimpleFS)
    add_custom_command(TARGET SimpleFS_a POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory out/
    )
    
    add_custom_command(TARGET SimpleFS_a POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SimpleFS_a> out/
    )
    
    add_custom_command(TARGET SimpleFS_so POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SimpleFS_so> out/
    )
    
    add_custom_command(TARGET SimpleFS_so POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/test/fs.c out/
    )
    
    add_custom_command(TARGET SimpleFS_so POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/include/fs_api.h out/
    )
    
    add_custom_command(TARGET SimpleFS_so POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/include/fs_typedefs.h out/
    )
    
    add_custom_command(TARGET SimpleFS_so POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/test/Makefile out/
    )
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Building on Windows")
    add_library(SimpleFS_static STATIC ${DIR_SRCS})
    add_library(SimpleFS_dll SHARED ${DIR_SRCS})
    set_property(TARGET SimpleFS_dll PROPERTY OUTPUT_NAME SimpleFS)
endif()
