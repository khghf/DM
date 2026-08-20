#ifndef COMMON_GLSL
#define COMMON_GLSL

// ============================================================
// API 检测
// ============================================================
#if defined(VULKAN)
    #define API_VULKAN 1
#elif defined(OPENGL)
    #define API_OPENGL 1
#elif defined(METAL)
    #define API_METAL 1
#else
    #define API_VULKAN 1  // 默认 Vulkan
#endif

// ============================================================
// 描述符集
// ============================================================
#ifdef API_VULKAN
    #define SET(x) set = x,
#else
    #define SET(x) 
#endif


#ifdef API_VULKAN
    #define PUSH_CONSTANT layout(push_constant) uniform
#else
    #define PUSH_CONSTANT layout(std140) uniform  // OpenGL 降级为 UBO
#endif


#define STD140 layout(std140)

#ifdef API_VULKAN
    #define INPUT_ATTACHMENT(set, binding, index) \
        layout(set = set, binding = binding, input_attachment_index = index) uniform subpassInput
#else
    #define INPUT_ATTACHMENT(set, binding, index) \
        layout(binding = binding) uniform sampler2D
#endif
#endif // COMMON_GLSL

// ============================================================
// 命名约定：dyn_ 表示动态资源，通常是动态分配的纹理、缓冲区等,不加 dyn_ 前缀的资源通常是静态资源，通常是静态分配的纹理、缓冲区等
// ============================================================