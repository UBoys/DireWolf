#include "direwolf/renderengine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

#include <chrono>

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

    // Smaller data set
    const float trianglePositionData[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f};

    // Taken from opengl-tutorial
    const GLfloat colorData[] = {
        0.583f, 0.771f, 0.014f, 1.0f,
        0.609f, 0.115f, 0.436f, 1.0f,
        0.327f, 0.483f, 0.844f, 1.0f,
        0.822f, 0.569f, 0.201f, 1.0f,
        0.435f, 0.602f, 0.223f, 1.0f,
        0.310f, 0.747f, 0.185f, 1.0f,
        0.597f, 0.770f, 0.761f, 1.0f,
        0.559f, 0.436f, 0.730f, 1.0f,
        0.359f, 0.583f, 0.152f, 1.0f,
        0.483f, 0.596f, 0.789f, 1.0f,
        0.559f, 0.861f, 0.639f, 1.0f,
        0.195f, 0.548f, 0.859f, 1.0f,
        0.014f, 0.184f, 0.576f, 1.0f,
        0.771f, 0.328f, 0.970f, 1.0f,
        0.406f, 0.615f, 0.116f, 1.0f,
        0.676f, 0.977f, 0.133f, 1.0f,
        0.971f, 0.572f, 0.833f, 1.0f,
        0.140f, 0.616f, 0.489f, 1.0f,
        0.997f, 0.513f, 0.064f, 1.0f,
        0.945f, 0.719f, 0.592f, 1.0f,
        0.543f, 0.021f, 0.978f, 1.0f,
        0.279f, 0.317f, 0.505f, 1.0f,
        0.167f, 0.620f, 0.077f, 1.0f,
        0.347f, 0.857f, 0.137f, 1.0f,
        0.055f, 0.953f, 0.042f, 1.0f,
        0.714f, 0.505f, 0.345f, 1.0f,
        0.783f, 0.290f, 0.734f, 1.0f,
        0.722f, 0.645f, 0.174f, 1.0f,
        0.302f, 0.455f, 0.848f, 1.0f,
        0.225f, 0.587f, 0.040f, 1.0f,
        0.517f, 0.713f, 0.338f, 1.0f,
        0.053f, 0.959f, 0.120f, 1.0f,
        0.393f, 0.621f, 0.362f, 1.0f,
        0.673f, 0.211f, 0.457f, 1.0f,
        0.820f, 0.883f, 0.371f, 1.0f,
        0.982f, 0.099f, 0.879f, 1.0f
    };

    const GLfloat positionData[] = {
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

    const uint32_t numVertices = (sizeof(positionData) / sizeof(float)) / VERTEX_LAYOUT;
    struct StandardVertex {
        glm::vec4 position;
        glm::vec4 color;
    };

    std::vector<StandardVertex> vertexData;
    // Convert raw data
    for (size_t i = 0; i < sizeof(positionData) / sizeof(float); i += VERTEX_LAYOUT) {
        glm::vec4 position = glm::vec4(positionData[i + 0], positionData[i + 1], positionData[i + 2], positionData[i + 3]);
        glm::vec4 color = glm::vec4(colorData[i + 0], colorData[i + 1], colorData[i + 2], colorData[i + 3]);
        StandardVertex vertex { position, color };
        vertexData.push_back(vertex);
    }

    // Sanity check
    // std::cerr << "NUM VERTICES " << numVertices << std::endl;
    // std::cerr << "SIZE OF VECTOR " << vertexData.size() << std::endl;
    // std::cerr << "SIZE OF STNADRAD VERTEX" << sizeof(StandardVertex) << std::endl;
    // std::cerr << "SIZE OF POS DATA " << sizeof(positionData) << std::endl;
    // std::cerr << "SIZE OF VECTOR ETC " << vertexData.size() * sizeof(StandardVertex) << std::endl;

    // Vertex data
    dw::GfxObject vertexBuffer;
    renderEngine->CreateVertexBuffer(vertexBuffer, numVertices);
    void* vb = renderEngine->MapVertexBuffer(vertexBuffer);
    std::memcpy(vb, vertexData.data(), vertexData.size() * sizeof(StandardVertex));
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

    // "Game loop"
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (true) {
        // Update
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        shaderData.model = glm::rotate(shaderData.model, 0.01f * deltaTime.count() * 35.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        lastTime = currentTime;

        // Render
        void* cb = renderEngine->MapConstantBuffer(constantBuffer);
        std::memcpy(cb, &shaderData, sizeof(shaderData));
        renderEngine->UnmapConstantBuffer(constantBuffer);
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
