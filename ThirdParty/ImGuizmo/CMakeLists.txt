# ==============================================================================
# ImGuizmo - 3D Gizmo 操作库 (静态库, 跨平台)
#   ImGuizmo 无官方 CMake, 自建构建逻辑
#   依赖 ImGui, 头文件通过 ImGuizmo/example 目录暴露
# ==============================================================================

set(TARGET_NAME "ImGuizmo")

add_library(${TARGET_NAME} STATIC
    "ImGuizmo/ImGuizmo.h"
    "ImGuizmo/ImGuizmo.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "${ThirdPartyName}"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${TARGET_NAME}"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             FILES
                "ImGuizmo/ImGuizmo.h"
                "ImGuizmo/ImGuizmo.cpp")

target_include_directories(${TARGET_NAME} PUBLIC
    "ImGuizmo/example"
)
