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

    std::cout << "\n\nPress enter to EXIT";
    std::cin.get();

	// Free the window callbacks and destroy the window.
	glfwDestroyWindow(window);
	glfwTerminate();

    return 0;
}
