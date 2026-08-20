set(TARGET_NAME ${ApplicationName})

# ==============================================================================
# 可执行文件入口
# ==============================================================================
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "Engine"
    OUTPUT_NAME "${ApplicationName}"
    RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${ApplicationName}"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             FILES ${HEADERS} ${SOURCES})

# 包含目录
target_include_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/${EngineName}/include"
    "$<$<CONFIG:Editor>:${CMAKE_SOURCE_DIR}/${EditorName}/include>"
    "include"
)

# 链接搜索路径
target_link_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${EngineName}"
    "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${GameName}"
    "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${EditorName}"
)

# 链接依赖
target_link_libraries(${TARGET_NAME} PRIVATE
    ${EngineName}
)

# Editor 库 (仅在 Editor 配置时链接)
target_link_libraries(${TARGET_NAME} PRIVATE
    $<$<CONFIG:Editor>:${EditorName}>
)
