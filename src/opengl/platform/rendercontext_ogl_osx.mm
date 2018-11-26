#if defined(__APPLE__)

#include "rendercontext_ogl_osx.h"
#include "common/config.h"

#include <iostream>
#include <Cocoa/Cocoa.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>

namespace {
    NSOpenGLContext* s_context;
    NSOpenGLView* s_view;
}

namespace dw {

RenderContextOSX::RenderContextOSX(const PlatformData& platformData) {
    std::cout << "Direwolf: Setting up render context for OSX\n";
    NSWindow* nsWindow = (NSWindow*)(platformData.windowHandle);
    assert(nsWindow && "Failed to initialize nsWindow\n");

    NSOpenGLPixelFormatAttribute profile = NSOpenGLProfileVersion3_2Core;
    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] = {
        NSOpenGLPFAOpenGLProfile, profile,
        NSOpenGLPFAColorSize,     24,
        NSOpenGLPFAAlphaSize,     8,
        NSOpenGLPFADepthSize,     24,
        NSOpenGLPFAStencilSize,   8,
        NSOpenGLPFADoubleBuffer,  true,
        NSOpenGLPFAAccelerated,   true,
        NSOpenGLPFANoRecovery,    true,
        0,                        0,
    };

    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
    assert(pixelFormat && "Failed to initialize pixel format\n");

    NSRect glViewRect = [[nsWindow contentView] bounds];
    NSOpenGLView* glView = [[NSOpenGLView alloc] initWithFrame:glViewRect pixelFormat:pixelFormat];
    assert(glView && "Failed to initialize glView\n");
    [pixelFormat release];

    // GLFW creates a helper contentView that handles things like keyboard and drag and
    // drop events. We don't want to clobber that view if it exists. Instead we just
    // add ourselves as a subview and make the view resize automatically.
    NSView *contentView = [nsWindow contentView];
    if (contentView) {
         [glView setAutoresizingMask:( NSViewHeightSizable |
                 NSViewWidthSizable |
                 NSViewMinXMargin |
                 NSViewMaxXMargin |
                 NSViewMinYMargin |
                 NSViewMaxYMargin )];
         [contentView addSubview:glView];
    }
    else {
        // TODO: Put assert here
        std::cerr << "WARNING: I don't think this should ever happen" << std::endl;
        [nsWindow setContentView:glView];
    }

    NSOpenGLContext* glContext = [glView openGLContext];
    assert(glContext && "Failed to initialize context\n");

    [glContext makeCurrentContext];
    GLint interval = 0;
    [glContext setValues:&interval forParameter:NSOpenGLCPSwapInterval];

    // When initializing NSOpenGLView programatically (as we are), this sometimes doesn't
    // get hooked up properly (especially when there are existing window elements). This ensures
    // we are valid. Otherwise, you'll probably get a GL_INVALID_FRA_EBUFFER_OPERATION when
    // trying to glClear() for the first time.
    [glContext setView:glView];

    s_view = glView;
    s_context = glContext;

    // Get the name of the video card.
    auto vendorString = static_cast<const uint8_t*>(glGetString(GL_VENDOR));
    auto versionString = static_cast<const uint8_t*>(glGetString(GL_VERSION));

    std::cout << "DireWolf: Successfully set up OpenGL context with GPU " << vendorString << std::endl;
    std::cout << "DireWolf: Running OpenGL version " << versionString << std::endl;
};

RenderContextOSX::~RenderContextOSX() {
    [s_view release];
    s_view = nullptr;
    s_context = nullptr;
}

void RenderContextOSX::SwapBuffers() const {
    [s_context makeCurrentContext];
    [s_context flushBuffer];
}

}  // namespace dw

#endif
