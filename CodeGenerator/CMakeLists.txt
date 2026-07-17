# ==============================================================================
# CodeGenerator - 代码生成工具 (控制台程序)
# 对应 premake: kind "ConsoleApp"
# 使用 LLVM libclang 解析 C++ 头文件, 自动生成反射代码
# ==============================================================================

set(TARGET_NAME "CodeGenerator")

# ==============================================================================
# 收集源文件: CodeGenerator/Src/ 目录
#   额外包含 LLVM 的 C API 头文件 (index.h), 用于 libclang 接口
# ==============================================================================
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp")

# ==============================================================================
# 创建控制台程序目标
#   同时把 libclang 的头文件也加入, 使其在 VS 中可见
# ==============================================================================
add_executable(${TARGET_NAME}
    ${HEADERS}
    ${SOURCES}
    "LLVM/include/clang-c/index.h"  # libclang C API 主头文件
)

# ==============================================================================
# 目标属性
#   CXX_STANDARD 20:          对单个目标设置 C++20 (覆盖全局设置)
#   CXX_STANDARD_REQUIRED ON:  强制要求 C++20
#   如果不需要覆盖全局, 可以不单独设置
# ==============================================================================
set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER "Engine"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>-${CMAKE_SYSTEM_NAME}-${CMAKE_GENERATOR_PLATFORM}/${TARGET_NAME}"
)

# ==============================================================================
# VS 筛选器
# ==============================================================================
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
             FILES ${HEADERS} ${SOURCES})

# ==============================================================================
# 包含目录
#   LLVM/include: libclang 头文件
#   Src/:          CodeGenerator 自己的源代码
#   json 头文件:   用于 JSON 输出
# ==============================================================================
target_include_directories(${TARGET_NAME} PRIVATE
    "LLVM/include"
    "Src"
    "${ThirdPartIncludeDir_json}"
)

# ==============================================================================
# 链接库搜索路径: LLVM 的 .lib 文件
# ==============================================================================
target_link_directories(${TARGET_NAME} PRIVATE
    "LLVM/lib"
)

# ==============================================================================
# 链接 libclang.lib (LLVM C API 静态库)
#   libclang 提供 C++ 代码解析、AST 遍历等能力
# ==============================================================================
target_link_libraries(${TARGET_NAME} PRIVATE
    libclang.lib
)

# ==============================================================================
# MSVC_RUNTIME_LIBRARY: 针对单个目标设置运行时库
#   生成器表达式 $<OR:条件1,条件2>:
#     Debug 或 Editor 任一满足时为真, 追加 "Debug" -> MultiThreadedDebugDLL (/MDd)
#     Release 时条件为假, 不加 "Debug" -> MultiThreadedDLL (/MD)
# ==============================================================================
set_target_properties(${TARGET_NAME} PROPERTIES
    MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Editor>>:Debug>DLL"
)
