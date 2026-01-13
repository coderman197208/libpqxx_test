project(subproject2)

# 查找源文件
file(GLOB SOURCES "src/*.cpp")

# 创建可执行文件 
add_executable(${PROJECT_NAME} ${SOURCES})

# 添加头文件包含路径
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include # 本地头文件路径
    PUBLIC
        ${COMMON_INCLUDE_DIR} # 公共头文件路径 
)

# 链接 libpqxx
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        libpqxx::pqxx      # PostgreSQL C++库
        yaml-cpp::yaml-cpp # yaml-cpp库
        spdlog::spdlog   # spdlog库
)
















# # subproject1/CMakeLists.txt

# #设置 PROJECT_NAME 变量为 subproject1
# project(subproject1)

# # 查找源文件
# file(GLOB SOURCES "src/*.cpp")

# # 创建可执行文件 
# add_executable(${PROJECT_NAME} ${SOURCES})

# # # 设置输出名称（目录由根项目控制）
# # set_target_properties(${PROJECT_NAME} PROPERTIES
# #     OUTPUT_NAME "spdlog_yaml_demo"
# # )

# # 添加头文件包含路径
# target_include_directories(${PROJECT_NAME}
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/include # 本地头文件路径
#     PUBLIC
#         ${COMMON_INCLUDE_DIR} # 公共头文件路径
#         ${COMMON_INCLUDE_DIR}/spdlog
#         ${COMMON_INCLUDE_DIR}/yaml-cpp 
#         # ${COMMON_INCLUDE_DIR}/pqxx 
# )

# # 链接 libpqxx
# target_link_libraries(${PROJECT_NAME}
#     PRIVATE
#         libpqxx::pqxx
#         ${PostgreSQL_LIBRARIES}
# )

# # # 如果使用独立编译的静态库，添加编译定义
# # target_compile_definitions(${PROJECT_NAME} PRIVATE 
# #     YAML_CPP_STATIC_DEFINE
# #     SPDLOG_COMPILED_LIB
# #     PQXX_STATIC_DEFINE
# # )

# # # 链接 spdlog 静态库
# # target_link_libraries(${PROJECT_NAME} PRIVATE ${SPDLOG_LIBRARY})
# # # 链接 yaml-cpp 静态库
# # target_link_libraries(${PROJECT_NAME} PRIVATE ${YAML_CPP_LIBRARY})
# # 链接 pqxx 静态库
# target_link_libraries(${PROJECT_NAME} PRIVATE ${PQXX_LIBRARY})


# # 可选：设置可执行文件属性
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     OUTPUT_NAME "subproject1"  # 确保输出名称一致
# )

# # # 可选：检查库文件是否存在
# # if(NOT EXISTS ${SPDLOG_LIBRARY})
# #     message(WARNING "spdlog library not found at: ${SPDLOG_LIBRARY}")
# # endif()

# message(STATUS "子项目 ${PROJECT_NAME} 配置完成")
# message(STATUS "源文件: ${SOURCES}")
# message(STATUS "源文件位置: ${CMAKE_CURRENT_SOURCE_DIR}/src")
# message(STATUS "可执行文件输出到: ${CMAKE_SOURCE_DIR}/bin/subproject1")