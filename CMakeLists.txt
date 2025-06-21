cmake_minimum_required(VERSION 3.14)
project(tiny_renderer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 查找SDL2库
find_package(SDL2 REQUIRED)

# 如果系统没有安装SDL2，则使用FetchContent下载
if(NOT SDL2_FOUND)
    message(STATUS "系统未找到SDL2，正在下载...")
    
    # 启用 FetchContent 模块
    include(FetchContent)

    # 配置 SDL2 依赖
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.28.5
        GIT_SHALLOW ON
    )

    # 根据平台设置 SDL2 编译选项
    if(WIN32)
        set(SDL2_BUILD_SHARED ON CACHE BOOL "" FORCE)
    else()
        set(SDL2_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    endif()

    # 禁用不必要的组件
    set(SDL2_DISABLE_SDL2MAIN OFF CACHE BOOL "" FORCE)
    set(SDL2_BUILD_TESTS OFF CACHE BOOL "" FORCE)

    # 使 SDL2 可用
    FetchContent_MakeAvailable(SDL2)
endif()

# 使用 ${PROJECT_NAME} 作为目标名称
add_executable(${PROJECT_NAME} 
    display.c
    main.c
    vector.c
    raytracer.c
)

# 链接SDL2库
if(SDL2_FOUND)
    # 使用系统安装的SDL2
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
else()
    # 使用FetchContent下载的SDL2
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE 
        SDL2::SDL2main
        SDL2::SDL2
        $<$<PLATFORM_ID:Linux>:m pthread>
        $<$<PLATFORM_ID:Darwin>:-framework Cocoa>
    )
endif()

# Windows 平台：复制 SDL2 DLL 到可执行文件目录
if(WIN32 AND NOT SDL2_FOUND)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "复制 SDL2 DLL 到可执行文件目录"
    )
endif()

# 调试信息
message(STATUS "${PROJECT_NAME} 配置完成")
if(SDL2_FOUND)
    message(STATUS "使用系统安装的SDL2")
    message(STATUS "SDL2 头文件目录: ${SDL2_INCLUDE_DIRS}")
else()
    message(STATUS "使用FetchContent下载的SDL2")
endif()