set(TARGET_NAME "ImGui")

add_library(${TARGET_NAME} STATIC
    "imgui/imconfig.h"
    "imgui/imgui.h"
    "imgui/imgui.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_internal.h"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/imstb_rectpack.h"
    "imgui/imstb_textedit.h"
    "imgui/imstb_truetype.h"
    "imgui/imgui_demo.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "${ThirdPartyName}"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${TARGET_NAME}"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             FILES
                "imgui/imconfig.h"
                "imgui/imgui.h"
                "imgui/imgui.cpp"
                "imgui/imgui_draw.cpp"
                "imgui/imgui_internal.h"
                "imgui/imgui_tables.cpp"
                "imgui/imgui_widgets.cpp"
                "imgui/imstb_rectpack.h"
                "imgui/imstb_textedit.h"
                "imgui/imstb_truetype.h"
                "imgui/imgui_demo.cpp")

