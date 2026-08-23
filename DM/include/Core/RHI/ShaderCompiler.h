#pragma once
#include <vector>
#include <string>
#include <memory>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_reflect.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include "RHITypes.h"

namespace DM::RHI
{
   

    /// <summary>
    /// 着色器编译结果
    /// </summary>
    struct CompiledShader
    {
        std::vector<uint32_t> spirv;
     
        std::string errorLog;
     
        bool success = false;
    };

   

    /// <summary>
    /// Include 处理器
    /// 负责解析 GLSL 中的 #include 指令，搜索并读取被包含的文件
    /// </summary>
    class IncludeHandler : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        /// <summary>
        /// 处理 #include 请求
        /// 搜索顺序：相对路径 → include 搜索路径 → 直接路径
        /// </summary>
        /// <param name="requested_source">被请求的文件名</param>
        /// <param name="type">include 类型（相对/系统）</param>
        /// <param name="requesting_source">发起请求的文件</param>
        /// <param name="include_depth">include 嵌套深度</param>
        /// <returns>包含的文件内容</returns>
        shaderc_include_result* GetInclude(
            const char* requested_source,
            shaderc_include_type type,
            const char* requesting_source,
            size_t include_depth) override;

        /// <summary>
        /// 释放 include 结果
        /// </summary>
        /// <param name="data">要释放的数据</param>
        void ReleaseInclude(shaderc_include_result* data) override;

        /// <summary>
        /// 添加 include 搜索路径
        /// </summary>
        /// <param name="path">搜索目录</param>
        void AddIncludePath(const std::string& path);

    private:
        /// <summary>
        /// 获取文件所在目录
        /// </summary>
        /// <param name="path">文件路径</param>
        /// <returns>目录路径</returns>
        std::string GetDirectory(const std::string& path);

        /// <summary>
        /// include 搜索路径列表
        /// </summary>
        std::vector<std::string> m_IncludePaths;
    };

    /// <summary>
    /// 着色器编译器
    /// 负责 GLSL 编译（支持 #include）和 SPIR-V 反射
    /// </summary>
    class ShaderCompiler
    {
    public:
        /// <summary>
        /// 构造函数：初始化编译器选项
        /// </summary>
        ShaderCompiler();

        ~ShaderCompiler() = default;

        /// <summary>
        /// 编译 GLSL 文件为 SPIR-V
        /// </summary>
        /// <param name="filename">GLSL 文件路径</param>
        /// <param name="stage">着色器阶段</param>
        /// <returns>编译结果</returns>
        CompiledShader CompileGLSLFileToSPV(const std::string& filename, EShaderStage stage);

        /// <summary>
        /// 编译 GLSL 源码为 SPIR-V
        /// </summary>
        /// <param name="source">GLSL 源码</param>
        /// <param name="stage">着色器阶段</param>
        /// <param name="filename">文件名（用于错误信息）</param>
        /// <returns>编译结果</returns>
        CompiledShader CompileGLSLSourceToSPV(
            const std::string& source,
            EShaderStage stage,
            const std::string& filename = "shader");

        /// <summary>
        /// 反射 SPIR-V，提取着色器接口信息
        /// </summary>
        /// <param name="spirv">SPIR-V 字节码</param>
        /// <param name="stage">着色器阶段</param>
        /// <returns>反射结果</returns>
        ShaderReflection Reflect(const std::vector<uint32_t>& spirv, EShaderStage stage);

        /// <summary>
        /// 添加 include 搜索路径
        /// </summary>
        /// <param name="path">搜索目录</param>
        void AddIncludePath(const std::string& path);

        /// <summary>
        /// 将 EShaderStage 转换为 shaderc 的着色器类型
        /// </summary>
        /// <param name="stage">引擎着色器阶段</param>
        /// <returns>shaderc 着色器类型</returns>
        shaderc_shader_kind GetShaderKind(EShaderStage stage);

    private:
        /// <summary>
        /// shaderc 编译器实例
        /// </summary>
        shaderc::Compiler m_Compiler;

        /// <summary>
        /// 编译选项（目标环境、优化级别等）
        /// </summary>
        shaderc::CompileOptions m_Options;

        /// <summary>
        /// include 处理器
        /// </summary>
        IncludeHandler* m_IncludeHandler;
    };

} // namespace DM::RHI