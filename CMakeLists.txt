cmake_minimum_required(VERSION 3.21)
project(DM LANGUAGES C CXX)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Editor" CACHE STRING "Available build configurations for DM Engine" FORCE)
# ==============================================================================
# C++ 标准
# ==============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_VULKAN_API "" ON)
option(ENABLE_OPENGL_API "" ON)




set(EngineName      "DM")
set(EditorName      "Editor")
set(GameName        "Game")
set(ApplicationName "Application")

# ==============================================================================
# 三方库根路径
# ==============================================================================
set(ThirdPartyName "ThirdParty")
set(ThirdPartyDir  "${CMAKE_SOURCE_DIR}/${ThirdPartyName}")

# ==============================================================================
# 跨平台架构标识 (用于输出目录命名)
#   CMAKE_GENERATOR_PLATFORM: VS 生成器填 x64/Win32, Ninja/Make 可能为空
#   CMAKE_SYSTEM_PROCESSOR:   兜底方案 (AMD64/arm64/x86_64 等)
#   统一结果: Windows-x64 / Darwin-arm64 / Linux-x86_64
# ==============================================================================
if(CMAKE_GENERATOR_PLATFORM)
    set(DM_PLATFORM_SUFFIX "${CMAKE_SYSTEM_NAME}-${CMAKE_GENERATOR_PLATFORM}")
else()
    set(DM_PLATFORM_SUFFIX "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

# ==============================================================================
# 平台检测
# ==============================================================================
set(DM_PLATFORM_WINDOWS OFF)
set(DM_PLATFORM_LINUX   OFF)
set(DM_PLATFORM_MACOS   OFF)
if(WIN32)
    message(STATUS "Configuring for windows")
    set(DM_PLATFORM_WINDOWS ON)
    add_compile_definitions(DM_PLATFORM_WINDOWS)
elseif(APPLE)
    message(FATAL_ERROR "Not supported platform:macos")
    set(DM_PLATFORM_MACOS ON)
    add_compile_definitions(DM_PLATFORM_MACOS)
elseif(UNIX)
    message(FATAL_ERROR "Not supported platform:linux")
    set(DM_PLATFORM_LINUX ON)
    add_compile_definitions(DM_PLATFORM_LINUX)
endif()

# ==============================================================================
# 三方库头文件目录
# ==============================================================================
set(IncludeDir_spdlog      "${ThirdPartyDir}/spdlog/include")
set(IncludeDir_glfw        "${ThirdPartyDir}/glfw/glfw/include")
set(IncludeDir_glad        "${ThirdPartyDir}/glad/glad/include")
set(IncludeDir_imgui       "${ThirdPartyDir}/imgui/imgui")
set(IncludeDir_glm         "${ThirdPartyDir}/glm/glm")
set(IncludeDir_stb_image   "${ThirdPartyDir}/stb_image")
set(IncludeDir_rapidjson   "${ThirdPartyDir}/rapidjson/include")
set(IncludeDir_json        "${ThirdPartyDir}/json")
set(IncludeDir_entt        "${ThirdPartyDir}/entt")
set(IncludeDir_yaml_cpp    "${ThirdPartyDir}/yaml_cpp/yaml_cpp/include")
#set(IncludeDir_ImGuizmo    "${ThirdPartyDir}/ImGuizmo/ImGuizmo")
#set(IncludeDir_shaderc     "${ThirdPartyDir}/shaderc/shaderc/include")
#set(IncludeDir_SPIRV_Cross "${ThirdPartyDir}/SPIRV-Cross/SPIRV-Cross")



# ==============================================================================
# 编译器选项
# ==============================================================================
if(MSVC)
    add_compile_options(/utf-8)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Editor>:Debug>DLL")
endif()

# ==============================================================================
# 全局预处理器宏
# ==============================================================================
add_compile_definitions(DM_DYNAMIC_LINK)
add_compile_definitions($<$<CONFIG:Debug>:DM_DEBUG>)
add_compile_definitions($<$<CONFIG:Release>:DM_RELEASE>)
add_compile_definitions($<$<CONFIG:Editor>:DM_DEBUG>)
add_compile_definitions($<$<CONFIG:Editor>:DM_EDITOR>)

# ==============================================================================
# 全局输出目录 (跨平台)
#   产物统一输出到 bin/<Config>-<Platform>/, 按目标名分子目录
# ==============================================================================
foreach(CONFIG_TYPE IN ITEMS Debug Release Editor)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE}
        "${CMAKE_SOURCE_DIR}/bin/${CONFIG_TYPE}-${DM_PLATFORM_SUFFIX}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE}
        "${CMAKE_SOURCE_DIR}/bin/${CONFIG_TYPE}-${DM_PLATFORM_SUFFIX}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE}
        "${CMAKE_SOURCE_DIR}/bin/${CONFIG_TYPE}-${DM_PLATFORM_SUFFIX}")
endforeach()

# ==============================================================================
# IDE 设置
# ==============================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${ApplicationName})

# ==============================================================================
# 子目录
# ==============================================================================
add_subdirectory(ThirdParty/glad)
add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/imgui)
add_subdirectory(ThirdParty/yaml_cpp)
#add_subdirectory(ThirdParty/ImGuizmo)
#add_subdirectory(ThirdParty/SPIRV-Cross)

add_subdirectory(${EngineName})
add_subdirectory(${EditorName})
add_subdirectory(${GameName})
add_subdirectory(${ApplicationName})
