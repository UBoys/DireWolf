#if defined(_WIN32)

#include "rendercontext_ogl_win.h"
#include "common/config.h"

#include <GL/glew.h>
#include <GL/wglew.h>
#include <iostream>

namespace {
    PIXELFORMATDESCRIPTOR m_pixelFormat;
    HGLRC m_renderContext;
    HDC m_deviceContext;
    HWND m_hwnd;
}

namespace dw {

// GLOBAL CLASS TODO: This looks like a mess 
RenderContextWin::RenderContextWin(const PlatformData& platformData) {
    int errorCode;
    HWND hwndFake = CreateWindowA("STATIC", "", WS_POPUP | WS_DISABLED, -32000, -32000, 0,
                                  0, NULL, NULL, GetModuleHandle(NULL), 0);

    // Initialize extensions
    // Get the device context
    m_deviceContext = GetDC(hwndFake);
    // Set a temporary pixel format
    errorCode = SetPixelFormat(m_deviceContext, 1, &m_pixelFormat);
    // Create temporary render context
    m_renderContext = wglCreateContext(m_deviceContext);
    // Set the temporary rendering context as the current rnedering context for this
    // window
    errorCode = wglMakeCurrent(m_deviceContext, m_renderContext);
    // Initialize the OpenGL extensions, we need a temp context to do so
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Direwolf: Failed to initialize extensions!";
    }

    // Release the temporary render context now that the extensions have loaded
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_renderContext);

    // Release device context
    ReleaseDC(hwndFake, m_deviceContext);

    // Get the device context again
    m_hwnd = static_cast<HWND>(platformData.windowHandle);
    m_deviceContext = GetDC(m_hwnd);

    int32_t attrs[] = {WGL_SAMPLE_BUFFERS_ARB,
                       0,
                       WGL_SAMPLES_ARB,
                       0,
                       WGL_SUPPORT_OPENGL_ARB,
                       true,
                       WGL_PIXEL_TYPE_ARB,
                       WGL_TYPE_RGBA_ARB,
                       WGL_DRAW_TO_WINDOW_ARB,
                       true,
                       WGL_DOUBLE_BUFFER_ARB,
                       true,
                       WGL_COLOR_BITS_ARB,
                       32,
                       WGL_DEPTH_BITS_ARB,
                       24,
                       WGL_STENCIL_BITS_ARB,
                       8,
                       0};

    int pixelFormat1[1];
    unsigned int formatCount;
    // Query for a pixel format that fits the attributes we want
    errorCode = wglChoosePixelFormatARB(m_deviceContext, attrs, NULL, 1, pixelFormat1,
                                        &formatCount);
    // If the gpu/display can handle the desired pixel format we set it as the current one
    errorCode = SetPixelFormat(m_deviceContext, pixelFormat1[0], &m_pixelFormat);
    int attributeList[5];
    // Set the 4.0 version of OpenGL in the attribute list.
    attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
    attributeList[1] = 4;
    attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
    attributeList[3] = 0;
    // Null terminate the attribute list.
    attributeList[4] = 0;
    m_renderContext = wglCreateContextAttribsARB(m_deviceContext, 0, attributeList);
    if (!m_renderContext) {
        std::cerr << "DireWolf: Failed to initialize OpenGL Context\n";
    }
    errorCode = wglMakeCurrent(m_deviceContext, m_renderContext);
    // Get the name of the video card.
	auto vendorString = static_cast<const unsigned char*>(glGetString(GL_VENDOR));
    auto versionString = static_cast<const unsigned char*>(glGetString(GL_VERSION));

	// TODO: Fetch from params
    if (/*vSync*/true) {
        wglSwapIntervalEXT(1);
    } else {
        wglSwapIntervalEXT(0);
    }

    std::cout << "DireWolf: Successfully set up OpenGL context with GPU " << vendorString << std::endl;
    std::cout << "DireWolf: Running OpenGL version " << versionString << std::endl;
}

}  // namespace dw

#endif