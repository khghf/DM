cmake_minimum_required(VERSION 3.21)
project(DM LANGUAGES C CXX)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Editor" CACHE STRING "Available build configurations for DM Engine" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_VULKAN_API "" ON)
option(ENABLE_OPENGL_API "" OFF)


set(EngineName      "DM")
set(EditorName      "Editor")
set(GameName        "Game")
set(ApplicationName "Application")


set(ThirdPartyName "ThirdParty")
set(ThirdPartyDir  "${CMAKE_SOURCE_DIR}/${ThirdPartyName}")


if(CMAKE_GENERATOR_PLATFORM)
    set(DM_PLATFORM_SUFFIX "${CMAKE_SYSTEM_NAME}-${CMAKE_GENERATOR_PLATFORM}")
else()
    set(DM_PLATFORM_SUFFIX "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()


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


set(IncludeDir_spdlog       "${ThirdPartyDir}/spdlog/include")
set(IncludeDir_glfw         "${ThirdPartyDir}/glfw/glfw/include")
set(IncludeDir_glad         "${ThirdPartyDir}/glad/glad/include")
set(IncludeDir_imgui        "${ThirdPartyDir}/imgui/imgui")
set(IncludeDir_glm          "${ThirdPartyDir}/glm/glm")
set(IncludeDir_stb_image    "${ThirdPartyDir}/stb_image")
set(IncludeDir_rapidjson    "${ThirdPartyDir}/rapidjson/include")
set(IncludeDir_json         "${ThirdPartyDir}/json")
set(IncludeDir_entt         "${ThirdPartyDir}/entt")
set(IncludeDir_yaml_cpp     "${ThirdPartyDir}/yaml_cpp/yaml_cpp/include")
set(IncludeDir_assimp       "${ThirdPartyDir}/assimp/assimp/include")
set(IncludeDir_PicoSHA2     "${ThirdPartyDir}/PicoSHA2")
set(IncludeDir_VMA          "${ThirdPartyDir}/VulkanMemoryAllocator/VulkanMemoryAllocator/include")
#set(IncludeDir_Renderdoc    "${ThirdPartyDir}/Renderdoc/api/app")


#set(IncludeDir_ImGuizmo    "${ThirdPartyDir}/ImGuizmo/ImGuizmo")



# ==============================================================================
# 编译器选项
# ==============================================================================
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    
    # ---------- Debug 配置 ----------
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od /RTC1 /MDd")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG:FULL /INCREMENTAL")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG:FULL /INCREMENTAL")
    
    # ---------- Editor 配置 ----------
    set(CMAKE_CXX_FLAGS_EDITOR "/Zi /Od /RTC1 /MDd /utf-8")
    set(CMAKE_EXE_LINKER_FLAGS_EDITOR "/DEBUG:FULL /INCREMENTAL")
    set(CMAKE_SHARED_LINKER_FLAGS_EDITOR "/DEBUG:FULL /INCREMENTAL")
    
    # ---------- Release 配置 ----------
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /MD")
    add_compile_options(
        $<$<CONFIG:Editor>:/MDd>
        $<$<CONFIG:Debug>:/MDd>
    )
endif()


add_compile_definitions(DM_DYNAMIC_LINK)
add_compile_definitions($<$<CONFIG:Debug>:DM_DEBUG>)
add_compile_definitions($<$<CONFIG:Release>:DM_RELEASE>)
add_compile_definitions($<$<CONFIG:Editor>:DM_DEBUG>)
add_compile_definitions($<$<CONFIG:Editor>:DM_EDITOR>)
if(ENABLE_VULKAN_API)
add_compile_definitions(ENABLE_VULKAN_API)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
elseif(ENABLE_OPENGL_API)
add_compile_definitions(ENABLE_OPENGL_API)
endif()


foreach(CONFIG_TYPE IN ITEMS Debug Release Editor)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE}
        "${CMAKE_SOURCE_DIR}/bin/${CONFIG_TYPE}-${DM_PLATFORM_SUFFIX}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE}
        "${CMAKE_SOURCE_DIR}/bin/${CONFIG_TYPE}-${DM_PLATFORM_SUFFIX}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE}
        "${CMAKE_SOURCE_DIR}/bin/${CONFIG_TYPE}-${DM_PLATFORM_SUFFIX}")
endforeach()


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${ApplicationName})


add_subdirectory(ThirdParty/glad)
add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/imgui)
add_subdirectory(ThirdParty/yaml_cpp)
add_subdirectory(ThirdParty/assimp)

add_subdirectory(${EngineName})
add_subdirectory(${GameName})
add_subdirectory(${ApplicationName})
add_subdirectory(${EditorName})