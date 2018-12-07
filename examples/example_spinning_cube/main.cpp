#include "direwolf/renderengine.h"

#include <iostream>
#include "GLFW/glfw3.h"

#if defined(_WIN32)
  #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
  #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

namespace {
    void _CallbackError(int error, const char* description) {} // TODO: Implement
    void* _GetGlfwNativeWindowhandle(GLFWwindow* window) {
#if defined(_WIN32)
        return static_cast<void*>(glfwGetWin32Window(window));
#elif defined(__APPLE__)
        return static_cast<void*>(glfwGetCocoaWindow(window));
#endif
    }

	GLFWwindow* s_window;

	void _setupWindow() {
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
		s_window = glfwCreateWindow(1280, 720, "Example Window", nullptr, nullptr);
		if (s_window == nullptr) {
			// TODO: Handle error
			glfwTerminate();
		}

		// Shows the glfw window.
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

    // Create a vertex buffer handle
    dw::GfxObject vertexBuffer;
    // Create the vertex buffer resource
    renderEngine->CreateVertexBuffer(vertexBuffer, /*count*/3);
    // Get the pointer to the vertex buffer
    float* data = static_cast<float*>(renderEngine->MapVertexBuffer(vertexBuffer));
    // An array of 3 vectors which represents 3 vertices
    const float vData[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };

    std::memcpy(data, vData, sizeof(vData));
    // Give the data back to the engine
    renderEngine->UnmapVertexBuffer(vertexBuffer);

    // Define the rendering commands (per frame)
    std::vector<dw::RenderCommand> renderCommands;
    // Data needed for the bind vertex buffer command (the handle) - so the renderer can look up the associated resource
    dw::BindVertexBufferCommandData vCommandData = { &vertexBuffer };
    // Data needed for the draw command (count and start vertex for now)
    dw::DrawCommandData drawCommand = { 3, 0 };

    // Give the per-frame commands to the render queue.
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
