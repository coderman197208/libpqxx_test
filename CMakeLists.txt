# spdlog_test/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(spdlog_test)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 检查是否支持 C++17 特性
if (CMAKE_CXX_STANDARD VERSION_LESS 17)
    message(WARNING "C++ standard is less than 17. query() method may not work.")
endif()

# 设置构建类型，Debug模式包含调试信息
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 设置编译选项，添加调试符号
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 查找 libpqxx
find_package(libpqxx REQUIRED)
message(STATUS "Found libpqxx: ${libpqxx_VERSION}")

# 查找 PostgreSQL
find_package(PostgreSQL REQUIRED)
message(STATUS "Found PostgreSQL: ${PostgreSQL_VERSION}")


# 设置公共头文件路径变量
set(COMMON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/common_include")

# 在 add_subdirectory 之前设置这些变量

# # 设置 spdlog 库文件路径变量
# set(SPDLOG_LIBRARY "${CMAKE_SOURCE_DIR}/lib/libspdlog.a")
# # 设置 yaml-cpp 库文件路径变量
# set(YAML_CPP_LIBRARY "${CMAKE_SOURCE_DIR}/lib/libyaml-cpp.a")
# 设置 pqxx 库文件路径变量
# set(PQXX_LIBRARY "${CMAKE_SOURCE_DIR}/lib/libpqxx.a")

# 添加所有子项目
add_subdirectory(subproject1)



# message(STATUS "yaml-cpp 库: ${YAML_CPP_LIBRARY}")
# message(STATUS "spdlog 库: ${SPDLOG_LIBRARY}")
message(STATUS "pqxx 库: ${PQXX_LIBRARY}")
message(STATUS "根目录 配置完成")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build 目录: ${CMAKE_BINARY_DIR}")
message(STATUS "可执行文件输出目录: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")