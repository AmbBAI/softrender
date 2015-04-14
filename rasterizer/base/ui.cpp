#include "ui.h"

#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg/src/nanovg_gl.h"
#include "nanovg/src/nanovg_gl_utils.h"

#define BLENDISH_IMPLEMENTATION
#include "blendish/blendish.h"

#define OUI_IMPLEMENTATION
#include "blendish/oui.h"

namespace rasterizer {

int UI::width = 0;
int UI::height = 0;
float UI::ratio = 1.f;
    
GLFWwindow* UI::window = nullptr;
UIcontext* UI::uictx = nullptr;
NVGcontext* UI::vg = nullptr;
    

typedef struct {
	int subtype;
	UIhandler handler;
} UIData;

void UIHandle(int item, UIevent event)
{
    UIData *data = (UIData *)uiGetHandle(item);
    if (data && data->handler) {
        data->handler(item, event);
    }
}

void UI::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    NVG_NOTUSED(scancode);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    uiSetKey(key, mods, action);
}
    
void UI::CharCallback(GLFWwindow *window, unsigned int value) {
    NVG_NOTUSED(window);
    uiSetChar(value);
}
    
void UI::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    NVG_NOTUSED(window);
    switch(button) {
        case 1: button = 2; break;
        case 2: button = 1; break;
    }
    uiSetButton(button, mods, (action==GLFW_PRESS)?1:0);
}
    
void UI::MousePositionCallback(GLFWwindow *window, double x, double y) {
    NVG_NOTUSED(window);
    uiSetCursor((int)x,(int)y);
}

void UI::ScrollCallback(GLFWwindow *window, double x, double y) {
    NVG_NOTUSED(window);
    uiSetScroll((int)x, (int)y);
}

    
bool UI::Initialize(GLFWwindow* window, int width, int height, float radio/* = 1.f*/)
{
    
#ifdef NANOVG_GL2_IMPLEMENTATION
    vg = nvgCreateGL2(NVG_ANTIALIAS);
#elif NANOVG_GL3_IMPLEMENTATION
    vg = nvgCreateGL3(NVG_ANTIALIAS);
#endif
    
    assert(vg != nullptr);
    if (vg == nullptr)  goto init_fail;
            
    uictx = uiCreateContext(4096, 1<<20);
    assert(uictx != nullptr);
    if (uictx == nullptr) goto init_fail;
    uiMakeCurrent(uictx);
    
    uiSetHandler(UIHandle);
    
    UI::width = width;
    UI::height = height;
    UI::window = window;
    
    // Init GLFW callback
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
    glfwSetCursorPosCallback(window, MousePositionCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    
    return true;
            
init_fail:
    Finalize();
    return false;
}

void UI::Finalize()
{
    if (vg != nullptr)
    {
#ifdef NANOVG_GL2_IMPLEMENTATION
        nvgDeleteGL2(vg);
#elif NANOVG_GL3_IMPLEMENTATION
        nvgDeleteGL3(vg);
#endif
        vg = nullptr;
    }
        
    if (uictx != nullptr)
    {
        uiDestroyContext(uictx);
        uictx = nullptr;
    }
}
    
void UI::SetFont(const char* file)
{
    assert(vg != nullptr);
    bndSetFont(nvgCreateFont(vg, "system", file));
}
void UI::SetIconImage(const char* file)
{
    assert(vg != nullptr);
    bndSetIconImage(nvgCreateImage(vg, file, 0));
}
    
void UI::Begin()
{
    assert(vg != nullptr);
    nvgBeginFrame(vg, width, height, ratio);
}
    
void UI::End()
{
    assert(vg != nullptr);
    nvgEndFrame(vg);
}
    
}