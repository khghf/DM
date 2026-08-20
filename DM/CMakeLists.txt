set(TARGET_NAME ${EngineName})

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp")#CONFIGURE_DEPENDS
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB STB_HEADERS  "${ThirdPartyDir}/stb_image/*.h")
file(GLOB STB_SOURCES  "${ThirdPartyDir}/stb_image/*.cpp")
file(GLOB_RECURSE JSON_HEADERS  "${ThirdPartyDir}/json/*.hpp")
file(GLOB_RECURSE ENTT_HEADERS  "${ThirdPartyDir}/entt/*.hpp")
file(GLOB VMA_HEADERS  "${IncludeDir_VMA}/*.h")




add_library(${TARGET_NAME} SHARED
    ${HEADERS}  ${SOURCES}
    ${STB_HEADERS} ${STB_SOURCES}
    ${JSON_HEADERS} ${ENTT_HEADERS}
    ${VMA_HEADERS}
)
add_compile_definitions(MIRROR_BUILD_DLL)

set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "Engine"
    RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${ApplicationName}"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${TARGET_NAME}"
)

target_precompile_headers(${TARGET_NAME} PRIVATE
     "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/include/DMPCH.h>")



target_include_directories(${TARGET_NAME} PUBLIC
    "./include"
    "${IncludeDir_spdlog}"
    "${IncludeDir_entt}"
    "${IncludeDir_glm}"
    "${IncludeDir_VMA}"
   # "${IncludeDir_Renderdoc}"
)

target_include_directories(${TARGET_NAME} PUBLIC
    "${IncludeDir_glfw}"
    "${IncludeDir_glad}"
    "${IncludeDir_stb_image}"
    "${IncludeDir_rapidjson}"
    "${IncludeDir_json}"
    "${IncludeDir_yaml_cpp}"
    "${IncludeDir_imgui}"
    "${IncludeDir_assimp}"
    "${IncludeDir_PicoSHA2}"
)
target_link_libraries(${TARGET_NAME} PRIVATE
    glfw
    glad
    yaml_cpp
    ImGui
    assimp
)


# 查找 renderdoc.dll
#find_file(RENDERDOC_DLL
#    NAMES renderdoc.dll
#    PATHS
#        "C:/Program Files/RenderDoc"
#        "C:/Program Files (x86)/RenderDoc"
#        "$ENV{PROGRAMFILES}/RenderDoc"
#        "${ThirdPartyDir}/renderdoc/lib"
#)
#    
#if(RENDERDOC_DLL)
#    message(STATUS "RenderDoc DLL found: ${RENDERDOC_DLL}")
#    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            "${RENDERDOC_DLL}"
#            $<TARGET_FILE_DIR:${TARGET_NAME}>/renderdoc.dll
#    )
#else()
#    message(WARNING "renderdoc.dll not found! Install RenderDoc from renderdoc.org")
#endif()



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
        # spirv-cross-glsl.lib
        $<$<CONFIG:Debug>:${VULKAN_SDK_DIR}/spirv-cross-glsld.lib>
        $<$<CONFIG:Release>:${VULKAN_SDK_DIR}/spirv-cross-glsl.lib>
        $<$<CONFIG:Editor>:${VULKAN_SDK_DIR}/spirv-cross-glsld.lib>
        # spirv-cross-reflect.lib
         $<$<CONFIG:Debug>:${VULKAN_SDK_DIR}/spirv-cross-reflectd.lib>
        $<$<CONFIG:Release>:${VULKAN_SDK_DIR}/spirv-cross-reflect.lib>
        $<$<CONFIG:Editor>:${VULKAN_SDK_DIR}/spirv-cross-reflectd.lib>
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




target_compile_definitions(${TARGET_NAME} PRIVATE
    DM_BUILD_SHARED
    $<$<PLATFORM_ID:Windows>:DM_BUILD_DLL>
    $<$<PLATFORM_ID:Windows>:GLFW_INCLUDE_NONE>
)
