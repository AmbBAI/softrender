#ifndef _BASE_UI_H_
#define _BASE_UI_H_

#include "base/header.h"
#include "base/color.h"

#include "nanovg/src/nanovg.h"
#include "blendish/blendish.h"
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

    static void test();
    
public:
	static int Panel(int parent, unsigned layout, int width, int height);
	static int Label(int parent, int icon, const char *label, Color color, unsigned layout = 0, int width = 0, int height = BND_WIDGET_HEIGHT);

	static int Box(unsigned layout, unsigned flags);
	static int Rect(const char* label, NVGcolor color);
	static int Button(int icon, const char *label, int *value);
	static int CheckBox(const char *label, int *option);
	static int RadioBox(int icon, const char *label, int *value);
	static int TextBox(char *text, int maxsize);
	static int Slider(const char *label, float *progress);

	static void BeginLayout();
	static void EndLayout();
	static void DrawUI(int item, int corners);

private:
	static void UIHandler(int item, UIevent event);
	static void ButtonHandler(int item, UIevent event);
	static void CheckHandler(int item, UIevent event);
	static void RadioBoxHandler(int item, UIevent event);
	static void SliderHandler(int item, UIevent event);
	static void TextBoxHandler(int item, UIevent event);

private:
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
