#ifndef _BASE_UI_H_
#define _BASE_UI_H_

#include "base/header.h"

#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg/src/nanovg.h"
#include "nanovg/src/nanovg_gl.h"

#define BLENDISH_IMPLEMENTATION
#include "blendish/blendish.h"

#define OUI_IMPLEMENTATION
#include "blendish/oui.h"

namespace rasterizer {

class UI
{
public:
    static bool Initialize(GLFWwindow* window, int width, int height, float radio = 1.f);
    static void Finalize();
    
    static void SetFont(const char* file);
    static void SetIconImage(const char* file);

    static void Begin();
    static void End();

private:
    typedef struct {
        int subtype;
        UIhandler handler;
    } UIData;
    
    static void UIHandle(int item, UIevent event);
    
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CharCallback(GLFWwindow* window, unsigned int value);
    static void MousePositionCallback(GLFWwindow* window, double x, double y);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void ScrollCallback(GLFWwindow* window, double x, double y);
    
private:
    static int width;
    static int height;
    static float ratio;

    static GLFWwindow* window;
    static UIcontext* uictx;
    static NVGcontext* vg;
};
    
}

#endif // !_BASE_UI_H_
