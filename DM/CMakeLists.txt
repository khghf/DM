set(TARGET_NAME ${EngineName})

# ==============================================================================
# 源文件收集
# ==============================================================================
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB STB_HEADERS  "${ThirdPartyDir}/stb_image/*.h")
file(GLOB STB_SOURCES  "${ThirdPartyDir}/stb_image/*.cpp")
file(GLOB_RECURSE JSON_HEADERS CONFIGURE_DEPENDS "${ThirdPartyDir}/json/*.hpp")
file(GLOB_RECURSE ENTT_HEADERS CONFIGURE_DEPENDS "${ThirdPartyDir}/entt/*.hpp")

# ==============================================================================
# 引擎核心动态库
# ==============================================================================
add_library(${TARGET_NAME} SHARED
    ${HEADERS}  ${SOURCES}
    ${STB_HEADERS} ${STB_SOURCES}
    ${JSON_HEADERS} ${ENTT_HEADERS}
)

# ==============================================================================
# 输出目录 (跨平台)
#   Windows: .dll → bin/, .lib → bin-int/
#   Linux:   .so  → bin/
#   macOS:   .dylib → bin/
# ==============================================================================
set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "Engine"
    RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${ApplicationName}"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${TARGET_NAME}"
)

# 预编译头
target_precompile_headers(${TARGET_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/include/DMPCH.h>"
)

# ==============================================================================
# 依赖配置
# ==============================================================================
target_include_directories(${TARGET_NAME} PUBLIC
    "./include"
    "${IncludeDir_spdlog}"
    "${IncludeDir_entt}"
    "${IncludeDir_glm}"
)

target_include_directories(${TARGET_NAME} PRIVATE
    "${IncludeDir_glfw}"
    "${IncludeDir_glad}"
    "${IncludeDir_stb_image}"
    "${IncludeDir_rapidjson}"
    "${IncludeDir_json}"
    "${IncludeDir_yaml_cpp}"
    "${IncludeDir_imgui}"
)
target_link_libraries(${TARGET_NAME} PRIVATE
    glfw
    glad
    yaml_cpp
    ImGui
    #ImGuizmo
)
if(ENABLE_VULKAN_API)
    if(DM_PLATFORM_WINDOWS)

        find_package(Vulkan REQUIRED)

        get_filename_component(VULKAN_SDK_DIR ${Vulkan_LIBRARY} DIRECTORY)

        target_include_directories(${TARGET_NAME} PRIVATE ${Vulkan_INCLUDE_DIR})

        target_link_libraries(${TARGET_NAME} PRIVATE 
        ${Vulkan_LIBRARY}

        #shaderc_combined.lib
        $<$<CONFIG:Debug>:${VULKAN_SDK_DIR}/shaderc_combinedd.lib>
        $<$<CONFIG:Release>:${VULKAN_SDK_DIR}/shaderc_combined.lib>
        $<$<CONFIG:Editor>:${VULKAN_SDK_DIR}/shaderc_combinedd.lib>
        #spirv-cross-core.lib
        $<$<CONFIG:Debug>:${VULKAN_SDK_DIR}/spirv-cross-cored.lib>
        $<$<CONFIG:Release>:${VULKAN_SDK_DIR}/spirv-cross-core.lib>
        $<$<CONFIG:Editor>:${VULKAN_SDK_DIR}/spirv-cross-cored.lib>
        # spirv-cross-glsl
        $<$<CONFIG:Debug>:${VULKAN_SDK_DIR}/spirv-cross-glsld.lib>
        $<$<CONFIG:Release>:${VULKAN_SDK_DIR}/spirv-cross-glsl.lib>
        $<$<CONFIG:Editor>:${VULKAN_SDK_DIR}/spirv-cross-glsld.lib>

        )
    elseif(DM_PLATFORM_LINUX)
    elseif(DM_PLATFORM_MACOS)
    endif()
endif()
if(ENABLE_OPENGL_API)
    if(DM_PLATFORM_WINDOWS)
        target_link_libraries(${TARGET_NAME} PRIVATE opengl32.lib)
    elseif(DM_PLATFORM_LINUX)
    elseif(DM_PLATFORM_MACOS)
    endif()
endif()



# ==============================================================================
# 预处理器宏
# ==============================================================================
target_compile_definitions(${TARGET_NAME} PRIVATE
    DM_BUILD_SHARED
    $<$<PLATFORM_ID:Windows>:DM_BUILD_DLL>
    $<$<PLATFORM_ID:Windows>:GLFW_INCLUDE_NONE>
)
