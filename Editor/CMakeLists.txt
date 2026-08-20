set(TARGET_NAME ${EditorName})

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(BACKEND_SOURCES "")
list(APPEND BACKEND_SOURCES
"${IncludeDir_imgui}/backends/imgui_impl_glfw.h"
"${IncludeDir_imgui}/backends/imgui_impl_glfw.cpp"
)

if(ENABLE_VULKAN_API)
    list(APPEND BACKEND_SOURCES 
        "${IncludeDir_imgui}/backends/imgui_impl_vulkan.h"
        "${IncludeDir_imgui}/backends/imgui_impl_vulkan.cpp")
endif()

if(ENABLE_OPENGL_API)
    list(APPEND BACKEND_SOURCES 
        "${IncludeDir_imgui}/backends/imgui_impl_opengl3.h"
        "${IncludeDir_imgui}/backends/imgui_impl_opengl3_loader.h"
        "${IncludeDir_imgui}/backends/imgui_impl_opengl3.cpp")
endif()
add_library(${TARGET_NAME} SHARED ${HEADERS} ${SOURCES} ${BACKEND_SOURCES})


# 输出目录
set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "Engine"
    RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${ApplicationName}"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${TARGET_NAME}"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             FILES ${HEADERS} ${SOURCES})

# 包含目录

if(ENABLE_VULKAN_API)
    if(DM_PLATFORM_WINDOWS)
    find_package(Vulkan REQUIRED)

    get_filename_component(VULKAN_SDK_DIR ${Vulkan_LIBRARY} DIRECTORY)

    target_include_directories(${TARGET_NAME} PRIVATE ${Vulkan_INCLUDE_DIR})

    target_link_libraries(${TARGET_NAME} PRIVATE 
    ${Vulkan_LIBRARY}
    )
    elseif(DM_PLATFORM_LINUX)
    elseif(DM_PLATFORM_MACOS)
    endif()
endif()
target_include_directories(${TARGET_NAME} PRIVATE
    "${IncludeDir_spdlog}"
    "${IncludeDir_glm}"
    "${IncludeDir_glfw}"
    "${IncludeDir_imgui}"
    "${IncludeDir_entt}"
    "${IncludeDir_yaml_cpp}"
    "include"
    "include/ImGui"
    "${CMAKE_SOURCE_DIR}/${EngineName}/include"
)





# 链接依赖
target_link_libraries(${TARGET_NAME} PRIVATE
    ${EngineName}
    yaml_cpp
    ImGui
    glfw
)


target_compile_definitions(${TARGET_NAME} PRIVATE
    EDITOR_BUILD_DLL
)


