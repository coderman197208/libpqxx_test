cmake_minimum_required(VERSION 3.10)
project(libpqxx_test)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


# 统一使用 find_package 查找所有第三方库
find_package(libpqxx REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)

# # 查找 libpqxx
# if(WIN32)
#     # Windows: 使用标准CMake查找
#     find_package(libpqxx REQUIRED)
#     find_package(yaml-cpp REQUIRED)
#     find_package(spdlog REQUIRED)
# else()
#     # Linux: 使用pkg-config
#     find_package(PkgConfig REQUIRED)
#     pkg_check_modules(LIBPQXX REQUIRED libpqxx)
#     pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
#     pkg_check_modules(SPDLOG REQUIRED spdlog)
    
#     # 创建统一的目标名称
#     add_library(libpqxx::pqxx INTERFACE IMPORTED)
#     target_include_directories(libpqxx::pqxx INTERFACE ${LIBPQXX_INCLUDE_DIRS})
#     target_link_libraries(libpqxx::pqxx INTERFACE ${LIBPQXX_LIBRARIES})

#     # 创建统一的目标名称
#     add_library(yaml-cpp::yaml-cpp INTERFACE IMPORTED)
#     target_include_directories(yaml-cpp::yaml-cpp INTERFACE ${YAML_CPP_INCLUDE_DIRS})
#     target_link_libraries(yaml-cpp::yaml-cpp INTERFACE ${YAML_CPP_LIBRARIES})

#     # 创建统一的目标名称
#     add_library(spdlog::spdlog INTERFACE IMPORTED)
#     target_include_directories(spdlog::spdlog INTERFACE ${SPDLOG_INCLUDE_DIRS})
#     target_link_libraries(spdlog::spdlog INTERFACE ${SPDLOG_LIBRARIES})

# endif()


# 设置公共头文件路径变量
set(COMMON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/common_include")

# 添加所有子项目
add_subdirectory(subproject1)
add_subdirectory(subproject2)


message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build 目录: ${CMAKE_BINARY_DIR}")
message(STATUS "可执行文件输出目录: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

