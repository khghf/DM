set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL      OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS     ON  CACHE BOOL "" FORCE)

add_subdirectory(yaml_cpp)
add_library(yaml_cpp ALIAS yaml-cpp)
if(TARGET yaml-cpp)
    message(STATUS "yaml-cpp target found!")
else()
    message(WARNING "yaml-cpp target NOT found!")
endif()

set_target_properties(yaml-cpp PROPERTIES
    FOLDER "${ThirdPartyName}"
    RUNTIME_OUTPUT_DIRECTORY
 	#"${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/yaml_cpp"

        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/$<IF:$<BOOL:${BUILD_SHARED_LIBS}>,${ApplicationName},yaml_cpp>"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/yaml_cpp"
)
message(STATUS "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/$<IF:$<BOOL:${BUILD_SHARED_LIBS}>,${ApplicationName},yaml_cpp>")