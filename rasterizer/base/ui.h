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
	typedef void(*UIButtonAction)();
	struct UIData {
		int type;
		UIhandler handler;
	};

public:
    static bool Initialize(GLFWwindow* window, int width, int height, float radio = 1.f);
    static void Finalize();
    
    static void SetFont(const char* file);
    static void SetIconImage(const char* file);

    static void Begin();
    static void End();
    
public:
	static int Root();
	static int Panel(int parent, unsigned layout, int width, int height);
	static int Box(int parent, unsigned flags, unsigned layout = 0);

	static int Label(int parent, int icon, const char *label, const Color& color, unsigned layout = UI_HFILL);
	static int Button(int parent, int icon, const char *label, UIButtonAction action, unsigned layout = UI_HFILL);
	static int Check(int parent, const char *label, int *option, unsigned layout = UI_HFILL);
	static int Radio(int parent, int icon, const char *label, int *value, unsigned layout = UI_HFILL);
	static int TextBox(int parent, char *text, int maxsize, unsigned layout = UI_HFILL);
	static int Slider(int parent, const char *label, float *progress, unsigned layout = UI_HFILL);

	static void DrawUI(int item, int corners);

private:
	static void UIHandler(int item, UIevent event);

private:
	static void DrawUIItems(int item, int corners);
	static void DrawUIItemsBox(int item);

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
    static NVGcontext* vgctx;
};
    
}

#endif // !_BASE_UI_H_
