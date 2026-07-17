set(TARGET_NAME ${EditorName})

# ==============================================================================
# 源文件收集
# ==============================================================================
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${TARGET_NAME} SHARED ${HEADERS} ${SOURCES})

# 输出目录
set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "Engine"
    LIBRARY_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${ApplicationName}"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${TARGET_NAME}"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             FILES ${HEADERS} ${SOURCES})

# 包含目录
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

# 预处理器宏
target_compile_definitions(${TARGET_NAME} PRIVATE
    EDITOR_BUILD_DLL
)


