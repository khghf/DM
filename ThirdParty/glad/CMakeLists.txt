set(TARGET_NAME "glad")

add_library(${TARGET_NAME} STATIC
    "glad/include/GLAD/glad.h"
    "glad/include/KHR/khrplatform.h"
    "glad/src/glad.c"
)
set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "${ThirdPartyName}"
    RUNTIME_OUTPUT_DIRECTORY
	"${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${TARGET_NAME}"
    ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${DM_PLATFORM_SUFFIX}/${TARGET_NAME}"
    C_STANDARD 11
)

target_include_directories(${TARGET_NAME} PUBLIC
    "glad/include"
)
