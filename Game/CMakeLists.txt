set(TARGET_NAME ${GameName})

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 空模块占位
if(NOT SOURCES AND NOT HEADERS)
    set(PLACEHOLDER "${CMAKE_CURRENT_SOURCE_DIR}/src/Placeholder.cpp")
    if(NOT EXISTS "${PLACEHOLDER}")
        file(WRITE "${PLACEHOLDER}" "// Placeholder - Game module source files go here\n")
    endif()
    set(SOURCES "${PLACEHOLDER}")
endif()

add_library(${TARGET_NAME} STATIC ${HEADERS} ${SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "Engine"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${TARGET_NAME}"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             FILES ${HEADERS} ${SOURCES})

target_include_directories(${TARGET_NAME} PUBLIC
    "${IncludeDir_spdlog}"
    "${IncludeDir_glm}"
    "${IncludeDir_glfw}"
    "${IncludeDir_IMGUI}"
    "${CMAKE_SOURCE_DIR}/${EngineName}/include"
)

target_link_libraries(${TARGET_NAME} PUBLIC
    ${EngineName}
)
