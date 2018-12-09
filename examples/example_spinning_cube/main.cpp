#include "direwolf/renderengine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

#if defined(_WIN32)
  #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
  #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

namespace {
    const uint16_t WINDOW_WIDTH = 1024;
    const uint16_t WINDOW_HEIGHT = 768;
    const uint8_t VERTEX_LAYOUT  = 4;

    const float vData[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f};

    const GLfloat vertexData[] = {
        -1.0f,-1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f, 1.0f,
        1.0f,-1.0f,-1.0f, 1.0f,
        1.0f, 1.0f,-1.0f, 1.0f,
        1.0f,-1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f, 1.0f,
        1.0f, 1.0f,-1.0f, 1.0f,
        1.0f,-1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f, 1.0f
    };

    void* _GetGlfwNativeWindowhandle(GLFWwindow* window) {
#if defined(_WIN32)
        return static_cast<void*>(glfwGetWin32Window(window));
#elif defined(__APPLE__)
        return static_cast<void*>(glfwGetCocoaWindow(window));
#endif
    }

    GLFWwindow* s_window;

    void _setupWindow() {
        if (!glfwInit()) {
            fprintf(stderr, "GLFW error: Failed to initialize!\n");
        }

        // Configures the window.
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // The window will stay hidden until after creation
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // The window will be resizable if not fullscreen
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Disable context creation

        // For new GLFW, and macOS.
        glfwWindowHint(GLFW_STENCIL_BITS, 8);	// Fixes 16 bit stencil bits in macOS.
        glfwWindowHint(GLFW_STEREO, GLFW_FALSE); // No stereo view!

        // Create a windowed mode window with no context
        s_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Example Window", nullptr, nullptr);
        if (s_window == nullptr) {
            glfwTerminate();
        }

        glfwShowWindow(s_window);
    }
}

int main() {
    std::cout << "Spinning cube example running\n";
    _setupWindow();

    // Setup renderer
    dw::InitData initData { dw::RendererType::RASTERIZER, dw::BackendType::OPENGL };
    dw::PlatformData platformData = { _GetGlfwNativeWindowhandle(s_window) };
    auto renderEngine = std::make_unique<dw::RenderEngine>(platformData, initData);

    // Create a pipeline state object
    /*dw::GfxObject pso;
    pso.vertexShader = "standard.vertex";
    pso.fragmentShader = "standard.fragment";
    pso.blendState = {};
    pso.rasterizerState = {};
    pso.depthStencilState = {};

    dw::GfxObject constantBuffer;
    constantBuffer.type = "";*/

    struct ShaderData {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    } shaderData;

    shaderData.model = glm::mat4(1.0f);
    shaderData.view = glm::lookAt(
        glm::vec3(4, 3, 3),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );
    shaderData.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);


    const uint32_t numVertices = (sizeof(vData) / sizeof(float)) / VERTEX_LAYOUT;

    // Create a vertex buffer handle
    dw::GfxObject vertexBuffer;
    // Create the vertex buffer resource
    renderEngine->CreateVertexBuffer(vertexBuffer, numVertices);
    // Get the pointer to the vertex buffer
    void* vb = renderEngine->MapVertexBuffer(vertexBuffer);
    // An array of 3 vectors which represents 3 vertices
    std::memcpy(vb, vData, sizeof(vData));
    // Give the data back to the engine
    renderEngine->UnmapVertexBuffer(vertexBuffer);

    // Set up constant buffer
    dw::GfxObject constantBuffer;
    renderEngine->CreateConstantBuffer(constantBuffer, sizeof(shaderData));
    void *cb = renderEngine->MapConstantBuffer(constantBuffer);
    std::memcpy(cb, &shaderData, sizeof(shaderData));
    renderEngine->UnmapConstantBuffer(constantBuffer);

    // Define the rendering commands (per frame)
    std::vector<dw::RenderCommand> renderCommands;
    // Data needed for the bind vertex buffer command (the handle) - so the renderer can look up the associated resource
    dw::BindVertexBufferCommandData vCommandData = { &vertexBuffer };
    // Data needed for the draw command (count and start vertex for now)
    dw::DrawCommandData drawCommand = { numVertices, /*offset=*/0 };
    dw::BindConstantBufferCommandData cCommandData = { &constantBuffer };

    // Give the per-frame commands to the render queue.
    renderCommands.push_back({ dw::BIND_CONSTANT_BUFFER, static_cast<void *>(&cCommandData)});
    renderCommands.push_back({ dw::BIND_VERTEX_BUFFER, static_cast<void*>(&vCommandData) });
    renderCommands.push_back({ dw::DRAW, static_cast<void*>(&drawCommand) });

    while (true) {
        // Dispatch render commands for a frame
        renderEngine->Render(renderCommands);
        // Window events
        glfwPollEvents();
    }


    std::cout << "\n\nPress enter to EXIT";
    std::cin.get();

    // Free the window callbacks and destroy the window.
    glfwDestroyWindow(s_window);
    glfwTerminate();

    return 0;
}
