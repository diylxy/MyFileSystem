# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.5)

# 项目信息
project (simpleFS)

set(CMAKE_BUILD_TYPE Debug)

aux_source_directory(src DIR_SRCS)

include_directories (include)
add_executable(simpleFS ${DIR_SRCS})
