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
    

typedef enum {
	ST_LABEL = 0,
	ST_BUTTON = 1,
	ST_RADIO = 2,
	ST_SLIDER = 3,
	ST_COLUMN = 4,
	ST_ROW = 5,
	ST_CHECK = 6,
	ST_PANEL = 7,
	ST_TEXT = 8,
	ST_IGNORE = 9,

	ST_DEMOSTUFF = 10,
	ST_RECT = 11,

	ST_BOX = 12,
} SubType;

typedef struct {
	int subtype;
	UIhandler handler;
} UIData;

typedef struct {
	UIData head;
	const char *label;
	NVGcolor color;
} UIRectData;

typedef struct {
	UIData head;
	int icon;
	const char *label;
	int *value;
} UIButtonData;

typedef struct  
{
	UIData head;
	int icon;
	const char *label;
	NVGcolor color;
} UILabelData;

typedef struct {
	UIData head;
	const char *label;
	int *option;
} UICheckData;

typedef struct {
	UIData head;
	int icon;
	const char *label;
	int *value;
} UIRadioData;

typedef struct {
	UIData head;
	const char *label;
	float *progress;
} UISliderData;

typedef struct {
	UIData head;
	char *text;
	int maxsize;
} UITextData;

void UI::UIHandler(int item, UIevent event)
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

#ifdef NANOVG_GLEW
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();
#endif
    
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
    
    uiSetHandler(UI::UIHandler);
    
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

void UI::DrawUIItems(int item, int corners) {
    int kid = uiFirstChild(item);
    while (kid > 0) {
        UI::DrawUI(kid, corners);
        kid = uiNextSibling(kid);
    }
}
    
void UI::DrawUIItemsBox(int item) {
    int kid = uiFirstChild(item);
    if (kid < 0) return;
    int nextkid = uiNextSibling(kid);
    if (nextkid < 0) {
        UI::DrawUI(kid, BND_CORNER_NONE);
    } else {
		unsigned flags = uiGetBox(item);
		unsigned corners = (flags & UI_ROW) ? BND_CORNER_LEFT : BND_CORNER_TOP;
		unsigned cornere = (flags & UI_ROW) ? BND_CORNER_RIGHT : BND_CORNER_DOWN;

		UI::DrawUI(kid, cornere);
        kid = nextkid;
        while (uiNextSibling(kid) > 0) {
            UI::DrawUI(kid, BND_CORNER_ALL);
            kid = uiNextSibling(kid);
        }
		UI::DrawUI(kid, corners);
    }
}
    
void UI::DrawUI(int item, int corners) {
	const UIData *head = (const UIData *)uiGetHandle(item);
	UIrect rect = uiGetRect(item);
	if (uiGetState(item) == UI_FROZEN) {
		nvgGlobalAlpha(vg, BND_DISABLED_ALPHA);
	}
	if (head) {
		switch (head->subtype) {
		default: {
			UI::DrawUIItems(item, corners);
		} break;
		case ST_BOX: {
			//bndBevel(vg, rect.x, rect.y, rect.w, rect.h);
			UI::DrawUIItemsBox(item);
		} break;
		case ST_PANEL: {
			//bndBevel(vg, rect.x, rect.y, rect.w, rect.h);
			UI::DrawUIItems(item, corners);
		} break;
		case ST_LABEL: {
			assert(head);
			const UILabelData *data = (UILabelData*)head;
			bndIconLabelValue(vg, rect.x, rect.y, rect.w, rect.h,
				data->icon, data->color, BND_LEFT, BND_LABEL_FONT_SIZE, data->label, nullptr);
		} break;
		case ST_BUTTON: {
			const UIButtonData *data = (UIButtonData*)head;
			bndToolButton(vg, rect.x, rect.y, rect.w, rect.h,
				corners, (BNDwidgetState)uiGetState(item),
				data->icon, data->label);
		} break;
		case ST_CHECK: {
			const UICheckData *data = (UICheckData*)head;
			BNDwidgetState state = (BNDwidgetState)uiGetState(item);
			if (*data->option)
				state = BND_ACTIVE;
			bndOptionButton(vg, rect.x, rect.y, rect.w, rect.h, state,
				data->label);
		} break;
		case ST_RADIO:{
			const UIRadioData *data = (UIRadioData*)head;
			BNDwidgetState state = (BNDwidgetState)uiGetState(item);
			if (*data->value == item)
				state = BND_ACTIVE;
			bndRadioButton(vg, rect.x, rect.y, rect.w, rect.h,
				corners, state,
				data->icon, data->label);
		} break;
		case ST_SLIDER:{
			const UISliderData *data = (UISliderData*)head;
			BNDwidgetState state = (BNDwidgetState)uiGetState(item);
			static char value[32];
			sprintf(value, "%.0f%%", (*data->progress)*100.0f);
			bndSlider(vg, rect.x, rect.y, rect.w, rect.h,
				corners, state,
				*data->progress, data->label, value);
		} break;
		case ST_TEXT: {
			const UITextData *data = (UITextData*)head;
			BNDwidgetState state = (BNDwidgetState)uiGetState(item);
			int idx = strlen(data->text);
			bndTextField(vg, rect.x, rect.y, rect.w, rect.h,
				corners, state, -1, data->text, idx, idx);
		} break;
		}
	}
	else {
		UI::DrawUIItems(item, corners);
	}

	if (uiGetState(item) == UI_FROZEN) {
		nvgGlobalAlpha(vg, 1.0);
	}
}

int UI::Label(int parent, int icon, const char *label, const Color& color, unsigned layout/* = 0*/) {
	int item = uiItem();
	UILabelData *data = (UILabelData *)uiAllocHandle(item, sizeof(UILabelData));
	data->head.subtype = ST_LABEL;
	data->head.handler = NULL;
	data->icon = icon;
	data->label = label;
	data->color = nvgRGBAf(color.r, color.g, color.b, color.a);

	if (parent >= 0) uiInsert(parent, item);
	uiSetSize(item, 0, BND_WIDGET_HEIGHT);
	uiSetLayout(item, layout);

	return item;
}
    
int UI::Button(int parent, int icon, const char *label, unsigned layout/* = 0*/) {
	int item = uiItem();
	UIButtonData *data = (UIButtonData *)uiAllocHandle(item, sizeof(UIButtonData));
	data->head.subtype = ST_BUTTON;
	data->head.handler = UI::ButtonHandler;
	data->icon = icon;
	data->label = label;

	if (parent >= 0) uiInsert(parent, item);
	uiSetSize(item, 0, BND_WIDGET_HEIGHT);
	uiSetLayout(item, layout);
	uiSetEvents(item, UI_BUTTON0_HOT_UP);

	return item;
}

void UI::ButtonHandler(int item, UIevent event) {
	const UIButtonData *data = (const UIButtonData *)uiGetHandle(item);
	printf("button %d clicked!\n", item);
}
    
void UI::CheckHandler(int item, UIevent event)
{
    const UICheckData *data = (const UICheckData *)uiGetHandle(item);
    *data->option = !(*data->option);
}
    
int UI::Check(int parent, const char *label, int *option, unsigned layout/* = 0*/)
{
    int item = uiItem();
    UICheckData *data = (UICheckData *)uiAllocHandle(item, sizeof(UICheckData));
    data->head.subtype = ST_CHECK;
	data->head.handler = UI::CheckHandler;
    data->label = label;
    data->option = option;

	if (parent >= 0) uiInsert(parent, item);
	uiSetSize(item, 0, BND_WIDGET_HEIGHT);
	uiSetLayout(item, layout);
	uiSetEvents(item, UI_BUTTON0_DOWN);

    return item;
}
    
// event handler for slider (same handler for all sliders)
void UI::SliderHandler(int item, UIevent event) {
    // retrieve the custom data we saved with the slider
    UISliderData *data = (UISliderData *)uiGetHandle(item);
	static float sliderstart = 0.0f;
    switch(event) {
        default: break;
        case UI_BUTTON0_DOWN: {
            // button was pressed for the first time; capture initial
            // slider value.
			sliderstart = *data->progress;
        } break;
        case UI_BUTTON0_CAPTURE: {
            // called for every frame that the button is pressed.
            // get the delta between the click point and the current
            // mouse position
            UIvec2 pos = uiGetCursorStartDelta();
            // get the items layouted rectangle
            UIrect rc = uiGetRect(item);
            // calculate our new offset and clamp
            float value = sliderstart + ((float)pos.x / (float)rc.w);
            value = (value<0)?0:(value>1)?1:value;
            // assign the new value
            *data->progress = value;
        } break;
    }
}
    
int UI::Slider(const char *label, float *progress) {
    // create new ui item
    int item = uiItem();
    // set size of wiget; horizontal size is dynamic, vertical is fixed
    uiSetSize(item, 0, BND_WIDGET_HEIGHT);
    // attach our slider event handler and capture two classes of events
    uiSetEvents(item, UI_BUTTON0_DOWN | UI_BUTTON0_CAPTURE);
    // store some custom data with the button that we use for styling
    // and logic, e.g. the pointer to the data we want to alter.
    UISliderData *data = (UISliderData *)uiAllocHandle(item, sizeof(UISliderData));
    data->head.subtype = ST_SLIDER;
    data->head.handler = UI::SliderHandler;
    data->label = label;
    data->progress = progress;
    return item;
}
    
void UI::TextBoxHandler(int item, UIevent event) {
    UITextData *data = (UITextData *)uiGetHandle(item);
    switch(event) {
        default: break;
        case UI_BUTTON0_DOWN: {
            uiFocus(item);
        } break;
        case UI_KEY_DOWN: {
            unsigned int key = uiGetKey();
            switch(key) {
                default: break;
                case GLFW_KEY_BACKSPACE: {
                    int size = strlen(data->text);
                    if (!size) return;
                    data->text[size-1] = 0;
                } break;
                case GLFW_KEY_ENTER: {
                    uiFocus(-1);
                } break;
            }
        } break;
        case UI_CHAR: {
            unsigned int key = uiGetKey();
            if ((key > 255)||(key < 32)) return;
            int size = strlen(data->text);
            if (size >= (data->maxsize-1)) return;
            data->text[size] = (char)key;
        } break;
    }
}
    
int UI::TextBox(char *text, int maxsize) {
    int item = uiItem();
    uiSetSize(item, 0, BND_WIDGET_HEIGHT);
    uiSetEvents(item, UI_BUTTON0_DOWN | UI_KEY_DOWN | UI_CHAR);
    // store some custom data with the button that we use for styling
    // and logic, e.g. the pointer to the data we want to alter.
    UITextData *data = (UITextData *)uiAllocHandle(item, sizeof(UITextData));
    data->head.subtype = ST_TEXT;
    data->head.handler = UI::TextBoxHandler;
    data->text = text;
    data->maxsize = maxsize;
    return item;
}
    
void UI::RadioHandler(int item, UIevent event)
{
	UIRadioData *data = (UIRadioData *)uiGetHandle(item);
	*(data->value) = item;
}

int UI::Radio(int parent, int icon, const char *label, int *value, unsigned layout/* = 0*/) {
    int item = uiItem();
    UIRadioData *data = (UIRadioData *)uiAllocHandle(item, sizeof(UIRadioData));
    data->head.subtype = ST_RADIO;
	data->head.handler = UI::RadioHandler;
    data->icon = icon;
    data->label = label;
    data->value = value;

	if (parent >= 0) uiInsert(parent, item);
	uiSetSize(item, label ? 0 : BND_TOOL_WIDTH, BND_WIDGET_HEIGHT);
	uiSetLayout(item, layout);
    uiSetEvents(item, UI_BUTTON0_DOWN);

    return item;
}
    
int UI::Panel(int parent, unsigned layout, int width, int height) {
	int item = uiItem();
	UIData *data = (UIData *)uiAllocHandle(item, sizeof(UIData));
	data->subtype = ST_PANEL;
	data->handler = NULL;

	if (parent >= 0) uiInsert(parent, item);
	uiSetSize(item, width, height);
	uiSetLayout(item, layout);

	return item;
}
    
int UI::Box(int parent, unsigned flags, unsigned layout/* = 0*/) {
	int item = uiItem();
	UIData *data = (UIData *)uiAllocHandle(item, sizeof(UIData));
	data->subtype = ST_BOX;
	data->handler = NULL;
	
	if (parent >= 0) uiInsert(parent, item);
	uiSetBox(item, flags);
	uiSetLayout(item, layout);

	return item;
}
    
void UI::BeginLayout()
{
	uiBeginLayout();
}

void UI::EndLayout()
{
	uiEndLayout();
}


}