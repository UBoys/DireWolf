#include "direwolf/renderengine.h"

#include <iostream>
#include "GLFW/glfw3.h"

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

namespace {
    void _CallbackError(int error, const char* description) {} // TODO: Implement
    void* _GetGlfwNativeWindowhandle(GLFWwindow* window) {
#if defined _WIN32
        return static_cast<void*>(glfwGetWin32Window(window));
#endif
    }
}

int main() {
    std::cout << "OpenGL example running\n";

    // Set up window
    glfwSetErrorCallback(_CallbackError);
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
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Example Window", nullptr, nullptr);
    if (window == nullptr) {
        // TODO: Handle error
        glfwTerminate();
    }

    // Shows the glfw window.
    glfwShowWindow(window);

    // Setup renderer
    dw::InitData initData { dw::RendererType::RASTERIZER, dw::BackendType::OPENGL };
    dw::PlatformData platformData = { _GetGlfwNativeWindowhandle(window) };
    auto renderEngine = std::make_unique<dw::RenderEngine>(platformData, initData);

    // Create a vertex buffer handle
    dw::GfxObject vertexBuffer;
    // Create the vertex buffer resource 
    renderEngine->CreateVertexBuffer(vertexBuffer, /*count*/3);
    // Get the pointer to the vertex buffer
    float* data = static_cast<float*>(renderEngine->MapVertexBuffer(vertexBuffer));

    // Ugly - fill it with some data
    data[0] = -1.0f;
    data[1] = -1.0f;
    data[2] = 0.0f;
    data[3] = 1.0f;

    data[4] = 1.0f;
    data[5] = -1.0f;
    data[6] = 0.0f;
    data[7] = 1.0f;

    data[8] = 0.0f;
    data[9] = 1.0f;
    data[10] = 0.0f;
    data[11] = 1.0f;

    // Give the data back to the engine
    renderEngine->UnmapVertexBuffer(vertexBuffer);

    // Define the rendering commands (per frame)
    std::vector<dw::RenderCommand> renderCommands;
    // Data needed for the bind vertex buffer command (the handle) - so the renderer can look up the associated resource
    dw::BindVertexBufferCommandData vCommandData = { &vertexBuffer };
    // Data needed for the draw command (count and start vertex for now)
    dw::DrawCommandData drawCommand = { 3, 0 };

    // Give the per-frame commands to the render queue.
    renderCommands.push_back({ dw::BindVertexBufferCommand, static_cast<void*>(&vCommandData) });
    renderCommands.push_back({ dw::DrawCommand, static_cast<void*>(&drawCommand) });

    while (true) {
        // Dispatch render commands for a frame
        renderEngine->Render(renderCommands);
        // Window events
        glfwPollEvents();
    }


    std::cout << "\n\nPress enter to EXIT";
    std::cin.get();

    // Free the window callbacks and destroy the window.
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
