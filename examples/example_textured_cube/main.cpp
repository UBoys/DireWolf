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

    // Copy pasta from opengl tutorial
    void* loadBMP(const char* path, unsigned int& imageSize, unsigned int& width, unsigned int& height) {
        printf("Reading image %s\n", path);

        // Data read from the header of the BMP file
        unsigned char header[54];
        unsigned int dataPos;
        // Actual RGB data
        unsigned char * data;

        // Open the file
        FILE* file = fopen(path,"rb");
        if (!file){
            printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", path);
            getchar();
            return 0;
        }

        // Read the header, i.e. the 54 first bytes

        // If less than 54 bytes are read, problem
        if ( fread(header, 1, 54, file)!=54 ){
            printf("Not a correct BMP file\n");
            fclose(file);
            return 0;
        }
        // A BMP files always begins with "BM"
        if ( header[0]!='B' || header[1]!='M' ){
            printf("Not a correct BMP file\n");
            fclose(file);
            return 0;
        }
        // Make sure this is a 24bpp file
        if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}
        if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}

        // Read the information about the image
        dataPos    = *(int*)&(header[0x0A]);
        imageSize  = *(int*)&(header[0x22]);
        width      = *(int*)&(header[0x12]);
        height     = *(int*)&(header[0x16]);

        // Some BMP files are misformatted, guess missing information
        if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
        if (dataPos==0)      dataPos=54; // The BMP header is done that way

        // Create a buffer
        data = new unsigned char [imageSize];

        // Read the actual data from the file into the buffer
        fread(data, 1, imageSize, file);

        // Everything is in memory now, the file can be closed.
        fclose (file);

        return data;
    }

    const float uvData[] = {
        0.000059f, 1.0f-0.000004f, 0.0f, 0.0f,
        0.000103f, 1.0f-0.336048f, 0.0f, 0.0f,
        0.335973f, 1.0f-0.335903f, 0.0f, 0.0f,
        1.000023f, 1.0f-0.000013f, 0.0f, 0.0f,
        0.667979f, 1.0f-0.335851f, 0.0f, 0.0f,
        0.999958f, 1.0f-0.336064f, 0.0f, 0.0f,
        0.667979f, 1.0f-0.335851f, 0.0f, 0.0f,
        0.336024f, 1.0f-0.671877f, 0.0f, 0.0f,
        0.667969f, 1.0f-0.671889f, 0.0f, 0.0f,
        1.000023f, 1.0f-0.000013f, 0.0f, 0.0f,
        0.668104f, 1.0f-0.000013f, 0.0f, 0.0f,
        0.667979f, 1.0f-0.335851f, 0.0f, 0.0f,
        0.000059f, 1.0f-0.000004f, 0.0f, 0.0f,
        0.335973f, 1.0f-0.335903f, 0.0f, 0.0f,
        0.336098f, 1.0f-0.000071f, 0.0f, 0.0f,
        0.667979f, 1.0f-0.335851f, 0.0f, 0.0f,
        0.335973f, 1.0f-0.335903f, 0.0f, 0.0f,
        0.336024f, 1.0f-0.671877f, 0.0f, 0.0f,
        1.000004f, 1.0f-0.671847f, 0.0f, 0.0f,
        0.999958f, 1.0f-0.336064f, 0.0f, 0.0f,
        0.667979f, 1.0f-0.335851f, 0.0f, 0.0f,
        0.668104f, 1.0f-0.000013f, 0.0f, 0.0f,
        0.335973f, 1.0f-0.335903f, 0.0f, 0.0f,
        0.667979f, 1.0f-0.335851f, 0.0f, 0.0f,
        0.335973f, 1.0f-0.335903f, 0.0f, 0.0f,
        0.668104f, 1.0f-0.000013f, 0.0f, 0.0f,
        0.336098f, 1.0f-0.000071f, 0.0f, 0.0f,
        0.000103f, 1.0f-0.336048f, 0.0f, 0.0f,
        0.000004f, 1.0f-0.671870f, 0.0f, 0.0f,
        0.336024f, 1.0f-0.671877f, 0.0f, 0.0f,
        0.000103f, 1.0f-0.336048f, 0.0f, 0.0f,
        0.336024f, 1.0f-0.671877f, 0.0f, 0.0f,
        0.335973f, 1.0f-0.335903f, 0.0f, 0.0f,
        0.667969f, 1.0f-0.671889f, 0.0f, 0.0f,
        1.000004f, 1.0f-0.671847f, 0.0f, 0.0f,
        0.667979f, 1.0f-0.335851f, 0.0f, 0.0f
    };

    const float positionData[] = {
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
        glm::vec4 uv;
    };

    std::vector<StandardVertex> vertexData;
    // Convert raw data
    for (size_t i = 0; i < sizeof(positionData) / sizeof(float); i += VERTEX_LAYOUT) {
        glm::vec4 position = glm::vec4(positionData[i + 0], positionData[i + 1], positionData[i + 2], positionData[i + 3]);
        glm::vec4 uv = glm::vec4(uvData[i + 0], uvData[i + 1], uvData[i + 2], uvData[i + 3]);
        StandardVertex vertex { position, uv };
        vertexData.push_back(vertex);
    }

    uint32_t imageSize;
    uint32_t width;
    uint32_t height;
    void* imageData = loadBMP("../../examples/example_textured_cube/uvtemplate.bmp", imageSize, width, height);

    // Texture data
    dw::GfxObject textureHandle;
    renderEngine->CreateTexture(textureHandle, {width, height, dw::PixelFormat::R8G8B8A8_UNORM}, imageData, imageSize);

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

    dw::BindTexturesCommandData bindTexturesCommandData = { { &textureHandle } };
    dw::BindConstantBufferCommandData cCommandData = { &constantBuffer };

    // Give the per-frame commands to the render queue.
    renderCommands.push_back({ dw::BIND_CONSTANT_BUFFER, static_cast<void *>(&cCommandData)});
    renderCommands.push_back({ dw::BIND_VERTEX_BUFFER, static_cast<void*>(&vCommandData) });
    renderCommands.push_back({ dw::BIND_TEXTURES, static_cast<void*>(&bindTexturesCommandData) });
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
