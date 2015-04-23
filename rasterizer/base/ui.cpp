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
    
    ////////////////////////////////////////////////////////////////////////////////
    
    void draw_demostuff(NVGcontext *vg, int x, int y, float w, float h);
    
    void drawUIItems(NVGcontext *vg, int item, int corners) {
        int kid = uiFirstChild(item);
        while (kid > 0) {
            UI::DrawUI(kid, corners);
            kid = uiNextSibling(kid);
        }
    }
    
    void drawUIItemsBox(NVGcontext *vg, int item) {
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
            switch(head->subtype) {
                default: {
                    drawUIItems(vg,item,corners);
                } break;
                case ST_BOX: {
                    drawUIItemsBox(vg, item);
                } break;
                case ST_PANEL: {
                    //bndBevel(vg,rect.x,rect.y,rect.w,rect.h);
                    drawUIItems(vg,item,corners);
                } break;
                case ST_LABEL: {
                    assert(head);
					const UILabelData *data = (UILabelData*)head;
					bndIconLabelValue(vg, rect.x, rect.y, rect.w, rect.h,
						data->icon, data->color, BND_LEFT, BND_LABEL_FONT_SIZE, data->label, nullptr);
                } break;
                case ST_BUTTON: {
                    const UIButtonData *data = (UIButtonData*)head;
                    bndToolButton(vg,rect.x,rect.y,rect.w,rect.h,
                                  corners,(BNDwidgetState)uiGetState(item),
                                  data->icon,data->label);
                } break;
                case ST_CHECK: {
                    const UICheckData *data = (UICheckData*)head;
                    BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                    if (*data->option)
                        state = BND_ACTIVE;
                    bndOptionButton(vg,rect.x,rect.y,rect.w,rect.h, state,
                                    data->label);
                } break;
                case ST_RADIO:{
                    const UIRadioData *data = (UIRadioData*)head;
                    BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                    if (*data->value == item)
                        state = BND_ACTIVE;
                    bndRadioButton(vg,rect.x,rect.y,rect.w,rect.h,
                                   corners,state,
                                   data->icon,data->label);
                } break;
                case ST_SLIDER:{
                    const UISliderData *data = (UISliderData*)head;
                    BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                    static char value[32];
                    sprintf(value,"%.0f%%",(*data->progress)*100.0f);
                    bndSlider(vg,rect.x,rect.y,rect.w,rect.h,
                              corners,state,
                              *data->progress,data->label,value);
                } break;
                case ST_TEXT: {
                    const UITextData *data = (UITextData*)head;
                    BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                    int idx = strlen(data->text);
                    bndTextField(vg,rect.x,rect.y,rect.w,rect.h,
                                 corners,state, -1, data->text, idx, idx);
                } break;
                case ST_DEMOSTUFF: {
                    draw_demostuff(vg, rect.x, rect.y, rect.w, rect.h);
                } break;
                case ST_RECT: {
                    const UIRectData *data = (UIRectData*)head;
                    if (rect.w && rect.h) {
                        BNDwidgetState state = (BNDwidgetState)uiGetState(item);
                        nvgSave(vg);
                        nvgStrokeColor(vg, nvgRGBAf(data->color.r,data->color.g,data->color.b,0.9f));
                        if (state != BND_DEFAULT) {
                            nvgFillColor(vg, nvgRGBAf(data->color.r,data->color.g,data->color.b,0.5f));
                        } else {
                            nvgFillColor(vg, nvgRGBAf(data->color.r,data->color.g,data->color.b,0.1f));
                        }
                        nvgStrokeWidth(vg,2);
                        nvgBeginPath(vg);
#if 0
                        nvgRect(vg,rect.x,rect.y,rect.w,rect.h);
#else
                        nvgRoundedRect(vg,rect.x,rect.y,rect.w,rect.h,3);
#endif
                        nvgFill(vg);
                        nvgStroke(vg);
                        
                        if (state != BND_DEFAULT) {
                            nvgFillColor(vg, nvgRGBAf(0.0f,0.0f,0.0f,1.0f));
                            nvgFontSize(vg, 15.0f);
                            nvgBeginPath(vg);
                            nvgTextAlign(vg, NVG_ALIGN_TOP|NVG_ALIGN_CENTER);
                            nvgTextBox(vg, rect.x, rect.y+rect.h*0.3f, rect.w, data->label, NULL);
                        }
                        
                        nvgRestore(vg);
                    }
                    nvgSave(vg);
                    nvgIntersectScissor(vg, rect.x, rect.y, rect.w, rect.h);
                    
                    drawUIItems(vg,item,corners);
                    
                    nvgRestore(vg);
                } break;
            }
        } else {
            drawUIItems(vg,item,corners);
        }
        
        if (uiGetState(item) == UI_FROZEN) {
            nvgGlobalAlpha(vg, 1.0);
        }
    }
    
    
int UI::Rect(const char *label, NVGcolor color) {
    int item = uiItem();
    UIRectData *data = (UIRectData *)uiAllocHandle(item, sizeof(UIRectData));
    data->head.subtype = ST_RECT;
    data->head.handler = NULL;
    data->label = label;
    data->color = color;
    uiSetEvents(item, UI_BUTTON0_DOWN);
    return item;
}
    
int UI::Label(int parent, int icon, const char *label, Color color,
	unsigned layout/* = 0*/, int width/* = 0*/, int height/* = BND_WIDGET_HEIGHT*/) {
	int item = uiItem();
	UILabelData *data = (UILabelData *)uiAllocHandle(item, sizeof(UILabelData));
	data->head.subtype = ST_LABEL;
	data->head.handler = NULL;
	data->icon = icon;
	data->label = label;
	data->color = nvgRGBAf(color.r, color.g, color.b, color.a);

	if (parent >= 0) uiInsert(parent, item);
	uiSetSize(item, width, height);
	uiSetLayout(item, layout);

	return item;
}
    
int UI::Button(int icon, const char *label, int *value) {
	// create new ui item
	int item = uiItem();
	// set size of wiget; horizontal size is dynamic, vertical is fixed
	uiSetSize(item, 0, BND_WIDGET_HEIGHT);
	uiSetEvents(item, UI_BUTTON0_HOT_UP);
	// store some custom data with the button that we use for styling
	UIButtonData *data = (UIButtonData *)uiAllocHandle(item, sizeof(UIButtonData));
	data->head.subtype = ST_BUTTON;
	data->head.handler = UI::ButtonHandler;
	data->icon = icon;
	data->label = label;
	return item;
}

void UI::ButtonHandler(int item, UIevent event) {
	const UIButtonData *data = (const UIButtonData *)uiGetHandle(item);
	if (data->value == nullptr) return;
	UIitemState state = uiGetState(item);
	switch (state)
	{
	case UI_ACTIVE:
		*(data->value) = 1;
		break;
	default:
		*(data->value) = 0;
		break;
	}
}
    
void UI::CheckHandler(int item, UIevent event) {
    const UICheckData *data = (const UICheckData *)uiGetHandle(item);
    *data->option = !(*data->option);
}
    
int UI::CheckBox(const char *label, int *option) {
    // create new ui item
    int item = uiItem();
    // set size of wiget; horizontal size is dynamic, vertical is fixed
    uiSetSize(item, 0, BND_WIDGET_HEIGHT);
    // attach event handler e.g. demohandler above
    uiSetEvents(item, UI_BUTTON0_DOWN);
    // store some custom data with the button that we use for styling
    UICheckData *data = (UICheckData *)uiAllocHandle(item, sizeof(UICheckData));
    data->head.subtype = ST_CHECK;
	data->head.handler = UI::CheckHandler;
    data->label = label;
    data->option = option;
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
    
void UI::RadioBoxHandler(int item, UIevent event)
{
	UIRadioData *data = (UIRadioData *)uiGetHandle(item);
	*(data->value) = item;
}

int UI::RadioBox(int icon, const char *label, int *value) {
    int item = uiItem();
    uiSetSize(item, label?0:BND_TOOL_WIDTH, BND_WIDGET_HEIGHT);
    UIRadioData *data = (UIRadioData *)uiAllocHandle(item, sizeof(UIRadioData));
    data->head.subtype = ST_RADIO;
	data->head.handler = UI::RadioBoxHandler;
    data->icon = icon;
    data->label = label;
    data->value = value;
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
    
int UI::Box(UIboxFlags flags) {
	int item = uiItem();
	UIData *data = (UIData *)uiAllocHandle(item, sizeof(UIData));
	data->subtype = ST_BOX;
	data->handler = NULL;
	uiSetBox(item, flags);
	return item;
}
    
    int column_append(int parent, int item) {
        uiInsert(parent, item);
        // fill parent horizontally, anchor to previous item vertically
        uiSetLayout(item, UI_HFILL);
        uiSetMargins(item, 0, 1, 0, 0);
        return item;
    }
    
    int column() {
        int item = uiItem();
        uiSetBox(item, UI_COLUMN);
        return item;
    }
    
    int vgroup_append(int parent, int item) {
        uiInsert(parent, item);
        // fill parent horizontally, anchor to previous item vertically
        uiSetLayout(item, UI_HFILL);
        return item;
    }
    
    int vgroup() {
        int item = uiItem();
        uiSetBox(item, UI_COLUMN);
        return item;
    }
    
    int hgroup_append(int parent, int item) {
        uiInsert(parent, item);
        uiSetLayout(item, UI_HFILL);
        return item;
    }
    
    int hgroup_append_fixed(int parent, int item) {
        uiInsert(parent, item);
        return item;
    }
    
    int hgroup() {
        int item = uiItem();
        uiSetBox(item, UI_ROW);
        return item;
    }
    
    int row_append(int parent, int item) {
        uiInsert(parent, item);
        uiSetLayout(item, UI_HFILL);
        return item;
    }
    
    int row() {
        int item = uiItem();
        uiSetBox(item, UI_ROW);
        return item;
    }
    
    void draw_noodles(NVGcontext *vg, int x, int y) {
        int w = 200;
        int s = 70;
        
        bndNodeBackground(vg, x+w, y-50, 100, 200, BND_DEFAULT, BND_ICONID(6,3),
                          "Default", nvgRGBf(0.392f,0.392f,0.392f));
        bndNodeBackground(vg, x+w+120, y-50, 100, 200, BND_HOVER, BND_ICONID(6,3),
                          "Hover", nvgRGBf(0.392f,0.392f,0.392f));
        bndNodeBackground(vg, x+w+240, y-50, 100, 200, BND_ACTIVE, BND_ICONID(6,3),
                          "Active", nvgRGBf(0.392f,0.392f,0.392f));
        
        for (int i = 0; i < 9; ++i) {
            int a = i%3;
            int b = i/3;
            bndNodeWire(vg, x, y+s*a, x+w, y+s*b, (BNDwidgetState)a, (BNDwidgetState)b);
        }
        
        bndNodePort(vg, x, y, BND_DEFAULT, nvgRGBf(0.5f, 0.5f, 0.5f));
        bndNodePort(vg, x+w, y, BND_DEFAULT, nvgRGBf(0.5f, 0.5f, 0.5f));
        bndNodePort(vg, x, y+s, BND_HOVER, nvgRGBf(0.5f, 0.5f, 0.5f));
        bndNodePort(vg, x+w, y+s, BND_HOVER, nvgRGBf(0.5f, 0.5f, 0.5f));
        bndNodePort(vg, x, y+2*s, BND_ACTIVE, nvgRGBf(0.5f, 0.5f, 0.5f));
        bndNodePort(vg, x+w, y+2*s, BND_ACTIVE, nvgRGBf(0.5f, 0.5f, 0.5f));
    }
    
    static void roothandler(int parent, UIevent event) {
        switch(event) {
            default: break;
            case UI_SCROLL: {
                UIvec2 pos = uiGetScroll();
                printf("scroll! %d %d\n", pos.x, pos.y);
            } break;
            case UI_BUTTON0_DOWN: {
                printf("%d clicks\n", uiGetClicks());
            } break;
        }
    }
    
    void draw_demostuff(NVGcontext *vg, int x, int y, float w, float h) {
        nvgSave(vg);
        nvgTranslate(vg, x, y);
        
        bndSplitterWidgets(vg, 0, 0, w, h);
        
        x = 10;
        y = 10;
        
        bndToolButton(vg,x,y,120,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_DEFAULT,
                      BND_ICONID(6,3),"Default");
        y += 25;
        bndToolButton(vg,x,y,120,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_HOVER,
                      BND_ICONID(6,3),"Hovered");
        y += 25;
        bndToolButton(vg,x,y,120,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_ACTIVE,
                      BND_ICONID(6,3),"Active");
        
        y += 40;
        bndRadioButton(vg,x,y,80,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_DEFAULT,
                       -1,"Default");
        y += 25;
        bndRadioButton(vg,x,y,80,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_HOVER,
                       -1,"Hovered");
        y += 25;
        bndRadioButton(vg,x,y,80,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_ACTIVE,
                       -1,"Active");
        
        y += 25;
        bndLabel(vg,x,y,120,BND_WIDGET_HEIGHT,-1,"Label:");
        y += BND_WIDGET_HEIGHT;
        bndChoiceButton(vg,x,y,80,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_DEFAULT,
                        -1, "Default");
        y += 25;
        bndChoiceButton(vg,x,y,80,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_HOVER,
                        -1, "Hovered");
        y += 25;
        bndChoiceButton(vg,x,y,80,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_ACTIVE,
                        -1, "Active");
        
        y += 25;
        int ry = y;
        int rx = x;
        
        y = 10;
        x += 130;
        bndOptionButton(vg,x,y,120,BND_WIDGET_HEIGHT,BND_DEFAULT,"Default");
        y += 25;
        bndOptionButton(vg,x,y,120,BND_WIDGET_HEIGHT,BND_HOVER,"Hovered");
        y += 25;
        bndOptionButton(vg,x,y,120,BND_WIDGET_HEIGHT,BND_ACTIVE,"Active");
        
        y += 40;
        bndNumberField(vg,x,y,120,BND_WIDGET_HEIGHT,BND_CORNER_DOWN,BND_DEFAULT,
                       "Top","100");
        y += BND_WIDGET_HEIGHT-2;
        bndNumberField(vg,x,y,120,BND_WIDGET_HEIGHT,BND_CORNER_ALL,BND_DEFAULT,
                       "Center","100");
        y += BND_WIDGET_HEIGHT-2;
        bndNumberField(vg,x,y,120,BND_WIDGET_HEIGHT,BND_CORNER_TOP,BND_DEFAULT,
                       "Bottom","100");
        
        int mx = x-30;
        int my = y-12;
        int mw = 120;
        bndMenuBackground(vg,mx,my,mw,120,BND_CORNER_TOP);
        bndMenuLabel(vg,mx,my,mw,BND_WIDGET_HEIGHT,-1,"Menu Title");
        my += BND_WIDGET_HEIGHT-2;
        bndMenuItem(vg,mx,my,mw,BND_WIDGET_HEIGHT,BND_DEFAULT,
                    BND_ICONID(17,3),"Default");
        my += BND_WIDGET_HEIGHT-2;
        bndMenuItem(vg,mx,my,mw,BND_WIDGET_HEIGHT,BND_HOVER,
                    BND_ICONID(18,3),"Hovered");
        my += BND_WIDGET_HEIGHT-2;
        bndMenuItem(vg,mx,my,mw,BND_WIDGET_HEIGHT,BND_ACTIVE,
                    BND_ICONID(19,3),"Active");
        
        y = 10;
        x += 130;
        int ox = x;
        bndNumberField(vg,x,y,120,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_DEFAULT,
                       "Default","100");
        y += 25;
        bndNumberField(vg,x,y,120,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_HOVER,
                       "Hovered","100");
        y += 25;
        bndNumberField(vg,x,y,120,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_ACTIVE,
                       "Active","100");
        
        y += 40;
        bndRadioButton(vg,x,y,60,BND_WIDGET_HEIGHT,BND_CORNER_RIGHT,BND_DEFAULT,
                       -1,"One");
        x += 60-1;
        bndRadioButton(vg,x,y,60,BND_WIDGET_HEIGHT,BND_CORNER_ALL,BND_DEFAULT,
                       -1,"Two");
        x += 60-1;
        bndRadioButton(vg,x,y,60,BND_WIDGET_HEIGHT,BND_CORNER_ALL,BND_DEFAULT,
                       -1,"Three");
        x += 60-1;
        bndRadioButton(vg,x,y,60,BND_WIDGET_HEIGHT,BND_CORNER_LEFT,BND_ACTIVE,
                       -1,"Butts");
        
        x = ox;
        y += 40;
        float progress_value = fmodf(glfwGetTime()/10.0,1.0);
        char progress_label[32];
        sprintf(progress_label, "%d%%", int(progress_value*100+0.5f));
        bndSlider(vg,x,y,240,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_DEFAULT,
                  progress_value,"Default",progress_label);
        y += 25;
        bndSlider(vg,x,y,240,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_HOVER,
                  progress_value,"Hovered",progress_label);
        y += 25;
        bndSlider(vg,x,y,240,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_ACTIVE,
                  progress_value,"Active",progress_label);
        
        int rw = x+240-rx;
        float s_offset = sinf(glfwGetTime()/2.0)*0.5+0.5;
        float s_size = cosf(glfwGetTime()/3.11)*0.5+0.5;
        
        bndScrollBar(vg,rx,ry,rw,BND_SCROLLBAR_HEIGHT,BND_DEFAULT,s_offset,s_size);
        ry += 20;
        bndScrollBar(vg,rx,ry,rw,BND_SCROLLBAR_HEIGHT,BND_HOVER,s_offset,s_size);
        ry += 20;
        bndScrollBar(vg,rx,ry,rw,BND_SCROLLBAR_HEIGHT,BND_ACTIVE,s_offset,s_size);
        
        const char edit_text[] = "The quick brown fox";
        int textlen = strlen(edit_text)+1;
        int t = int(glfwGetTime()*2);
        int idx1 = (t/textlen)%textlen;
        int idx2 = idx1 + (t%(textlen-idx1));
        
        ry += 25;
        bndTextField(vg,rx,ry,240,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_DEFAULT,
                     -1, edit_text, idx1, idx2);
        ry += 25;
        bndTextField(vg,rx,ry,240,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_HOVER,
                     -1, edit_text, idx1, idx2);
        ry += 25;
        bndTextField(vg,rx,ry,240,BND_WIDGET_HEIGHT,BND_CORNER_NONE,BND_ACTIVE,
                     -1, edit_text, idx1, idx2);
        
        draw_noodles(vg, 20, ry+50);
        
        rx += rw + 20;
        ry = 10;
        bndScrollBar(vg,rx,ry,BND_SCROLLBAR_WIDTH,240,BND_DEFAULT,s_offset,s_size);
        rx += 20;
        bndScrollBar(vg,rx,ry,BND_SCROLLBAR_WIDTH,240,BND_HOVER,s_offset,s_size);
        rx += 20;
        bndScrollBar(vg,rx,ry,BND_SCROLLBAR_WIDTH,240,BND_ACTIVE,s_offset,s_size);
        
        x = ox;
        y += 40;
        bndToolButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_RIGHT,
                      BND_DEFAULT,BND_ICONID(0,10),NULL);
        x += BND_TOOL_WIDTH-1;
        bndToolButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_ALL,
                      BND_DEFAULT,BND_ICONID(1,10),NULL);
        x += BND_TOOL_WIDTH-1;
        bndToolButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_ALL,
                      BND_DEFAULT,BND_ICONID(2,10),NULL);
        x += BND_TOOL_WIDTH-1;
        bndToolButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_ALL,
                      BND_DEFAULT,BND_ICONID(3,10),NULL);
        x += BND_TOOL_WIDTH-1;
        bndToolButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_ALL,
                      BND_DEFAULT,BND_ICONID(4,10),NULL);
        x += BND_TOOL_WIDTH-1;
        bndToolButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_LEFT,
                      BND_DEFAULT,BND_ICONID(5,10),NULL);
        x += BND_TOOL_WIDTH-1;
        x += 5;
        bndRadioButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_RIGHT,
                       BND_DEFAULT,BND_ICONID(0,11),NULL);
        x += BND_TOOL_WIDTH-1;
        bndRadioButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_ALL,
                       BND_DEFAULT,BND_ICONID(1,11),NULL);
        x += BND_TOOL_WIDTH-1;
        bndRadioButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_ALL,
                       BND_DEFAULT,BND_ICONID(2,11),NULL);
        x += BND_TOOL_WIDTH-1;
        bndRadioButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_ALL,
                       BND_DEFAULT,BND_ICONID(3,11),NULL);
        x += BND_TOOL_WIDTH-1;
        bndRadioButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_ALL,
                       BND_ACTIVE,BND_ICONID(4,11),NULL);
        x += BND_TOOL_WIDTH-1;
        bndRadioButton(vg,x,y,BND_TOOL_WIDTH,BND_WIDGET_HEIGHT,BND_CORNER_LEFT,
                       BND_DEFAULT,BND_ICONID(5,11),NULL);
        
        nvgRestore(vg);
    }
    
    static int enum1 = -1;
    
    void build_democontent(int parent) {
        // some persistent variables for demonstration
        static float progress1 = 0.25f;
        static float progress2 = 0.75f;
        static int option1 = 1;
        static int option2 = 0;
        static int option3 = 0;
		static int btn1 = 0;
		static int btn2 = 0;
		static int btn3 = 0;
		static int btn4 = 0;

        int col = column();
        uiInsert(parent, col);
        uiSetMargins(col, 10, 10, 10, 10);
        uiSetLayout(col, UI_TOP|UI_HFILL);
        
        column_append(col, UI::Button(BND_ICON_GHOST, "Item 1", &btn1));
        if (option3)
			column_append(col, UI::Button(BND_ICON_GHOST, "Item 2", &btn2));
        
        {
            int h = column_append(col, UI::Box(UI_ROW));
            hgroup_append(h, UI::RadioBox(BND_ICON_GHOST, "Item 3.0", &enum1));
            if (option2)
				uiSetMargins(hgroup_append_fixed(h, UI::RadioBox(BND_ICON_REC, NULL, &enum1)), -1, 0, 0, 0);
			uiSetMargins(hgroup_append_fixed(h, UI::RadioBox(BND_ICON_PLAY, NULL, &enum1)), -1, 0, 0, 0);
			uiSetMargins(hgroup_append(h, UI::RadioBox(BND_ICON_GHOST, "Item 3.3", &enum1)), -1, 0, 0, 0);
        }
        
        {
            int rows = column_append(col, row());
            int coll = row_append(rows, vgroup());
			vgroup_append(coll, UI::Label(coll, -1, "Items 4.0:", Color::black));
            coll = vgroup_append(coll, UI::Box(UI_COLUMN));
            vgroup_append(coll, UI::Button(BND_ICON_GHOST, "Item 4.0.0", &btn3));
			uiSetMargins(vgroup_append(coll, UI::Button(BND_ICON_GHOST, "Item 4.0.1", &btn4)), 0, -2, 0, 0);
            int colr = row_append(rows, vgroup());
            uiSetMargins(colr, 8, 0, 0, 0);
            uiSetFrozen(colr, option1);
			vgroup_append(colr, UI::Label(coll, -1, "Items 4.1:", Color::black));
			colr = vgroup_append(colr, UI::Box(UI_COLUMN));
            vgroup_append(colr, UI::Slider("Item 4.1.0", &progress1));
            uiSetMargins(vgroup_append(colr, UI::Slider("Item 4.1.1", &progress2)),0,-2,0,0);
        }
        
        column_append(col, UI::Button(BND_ICON_GHOST, "Item 5", NULL));
        
        static char textbuffer[1024] = "The quick brown fox.";
        column_append(col, UI::TextBox(textbuffer, 1024));
        
        column_append(col, UI::CheckBox("Frozen", &option1));
		column_append(col, UI::CheckBox("Item 7", &option2));
		column_append(col, UI::CheckBox("Item 8", &option3));
    }
    
    int demorect(int parent, const char *label, float hue, int box, int layout, int w, int h, int m1, int m2, int m3, int m4) {
        int item = UI::Rect(label, nvgHSL(hue, 1.0f, 0.8f));
        uiSetLayout(item, layout);
        uiSetBox(item, box);
        uiSetMargins(item, m1, m2, m3, m4);
        uiSetSize(item, w, h);
        uiInsert(parent, item);
        return item;
    }
    
    void build_layoutdemo(int parent) {
        const int M = 10;
        const int S = 150;
        
        int box = demorect(parent, "Box( UI_LAYOUT )\nLayout( UI_FILL )", 0.6f, UI_LAYOUT, UI_FILL, 0, 0, M, M, M, M);
        demorect(box, "Layout( UI_HFILL | UI_TOP )", 0.7f, 0, UI_HFILL|UI_TOP, S, S+M, M, M, M, 0);
        demorect(box, "Layout( UI_HFILL )", 0.7f, 0, UI_HFILL, S, S+2*M, M, 0, M, 0);
        demorect(box, "Layout( UI_HFILL | UI_DOWN )", 0.7f, 0, UI_HFILL|UI_DOWN, S, S+M, M, 0, M, M);
        
        demorect(box, "Layout( UI_LEFT | UI_VFILL )", 0.7f, 0, UI_LEFT|UI_VFILL, S+M, S, M, M, 0, M);
        demorect(box, "Layout( UI_VFILL )", 0.7f, 0, UI_VFILL, S+2*M, S, 0, M, 0, M);
        demorect(box, "Layout( UI_RIGHT | UI_VFILL )", 0.7f, 0, UI_RIGHT|UI_VFILL, S+M, S, 0, M, M, M);
        
        demorect(box, "Layout( UI_LEFT | UI_TOP )", 0.55f, 0, UI_LEFT|UI_TOP, S, S, M, M, 0, 0);
        demorect(box, "Layout( UI_TOP )", 0.57f, 0, UI_TOP, S, S, 0, M, 0, 0);
        demorect(box, "Layout( UI_RIGHT | UI_TOP )", 0.55f, 0, UI_RIGHT|UI_TOP, S, S, 0, M, M, 0);
        demorect(box, "Layout( UI_LEFT )", 0.57f, 0, UI_LEFT, S, S, M, 0, 0, 0);
        demorect(box, "Layout( UI_CENTER )", 0.59f, 0, UI_CENTER, S, S, 0, 0, 0, 0);
        demorect(box, "Layout( UI_RIGHT )", 0.57f, 0, UI_RIGHT, S, S, 0, 0, M, 0);
        demorect(box, "Layout( UI_LEFT | UI_DOWN )", 0.55f, 0, UI_LEFT|UI_DOWN, S, S, M, 0, 0, M);
        demorect(box, "Layout( UI_DOWN)", 0.57f, 0, UI_DOWN, S, S, 0, 0, 0, M);
        demorect(box, "Layout( UI_RIGHT | UI_DOWN )", 0.55f, 0, UI_RIGHT|UI_DOWN, S, S, 0, 0, M, M);
    }
    
    void build_rowdemo(int parent) {
        uiSetBox(parent, UI_COLUMN);
        
        const int M = 10;
        const int S = 200;
        const int T = 100;
        
        {
            int box = demorect(parent, "Box( UI_ROW )\nLayout( UI_LEFT | UI_VFILL )", 0.6f, UI_ROW, UI_LEFT|UI_VFILL, 0, S, M, M, M, M);
            
            demorect(box, "Layout( UI_TOP )", 0.05f, 0, UI_TOP, T, T, M, M, M, 0);
            demorect(box, "Layout( UI_VCENTER )", 0.1f, 0, UI_VCENTER, T, T, 0, 0, M, 0);
            demorect(box, "Layout( UI_VFILL )", 0.15f, 0, UI_VFILL, T, T, 0, M, M, M);
            demorect(box, "Layout( UI_DOWN )", 0.25f, 0, UI_DOWN, T, T, 0, 0, M, M);
        }
        {
            int box = demorect(parent, "Box( UI_ROW | UI_JUSTIFY )\nLayout( UI_FILL )", 0.6f, UI_ROW|UI_JUSTIFY, UI_FILL, 0, S, M, 0, M, M);
            
            demorect(box, "Layout( UI_TOP )", 0.05f, 0, UI_TOP, T, T, M, M, M, 0);
            demorect(box, "Layout( UI_VCENTER )", 0.1f, 0, UI_VCENTER, T, T, 0, 0, M, 0);
            demorect(box, "Layout( UI_VFILL )", 0.15f, 0, UI_VFILL, T, T, 0, M, M, M);
            demorect(box, "Layout( UI_DOWN )", 0.25f, 0, UI_DOWN, T, T, 0, 0, M, M);
        }
        {
            int box = demorect(parent, "Box( UI_ROW )\nLayout( UI_FILL )", 0.6f, UI_ROW, UI_FILL, 0, S, M, 0, M, M);
            
            demorect(box, "Layout( UI_TOP )", 0.05f, 0, UI_TOP, T, T, M, M, M, 0);
            demorect(box, "Layout( UI_VCENTER )", 0.1f, 0, UI_VCENTER, T, T, 0, 0, M, 0);
            demorect(box, "Layout( UI_VFILL )", 0.15f, 0, UI_VFILL, T, T, 0, M, M, M);
            demorect(box, "Layout( UI_HFILL )", 0.2f, 0, UI_HFILL, T, T, 0, 0, M, 0);
            demorect(box, "Layout( UI_HFILL )", 0.2f, 0, UI_HFILL, T, T, 0, 0, M, 0);
            demorect(box, "Layout( UI_HFILL )", 0.2f, 0, UI_HFILL, T, T, 0, 0, M, 0);
            demorect(box, "Layout( UI_DOWN )", 0.25f, 0, UI_DOWN, T, T, 0, 0, M, M);
        }
    }
    
    void build_columndemo(int parent) {
        uiSetBox(parent, UI_ROW);
        
        const int M = 10;
        const int S = 200;
        const int T = 100;
        
        {
            int box = demorect(parent, "Box( UI_COLUMN )\nLayout( UI_TOP | UI_HFILL )", 0.6f, UI_COLUMN, UI_TOP|UI_HFILL, S, 0, M, M, M, M);
            
            demorect(box, "Layout( UI_LEFT )", 0.05f, 0, UI_LEFT, T, T, M, M, 0, M);
            demorect(box, "Layout( UI_HCENTER )", 0.1f, 0, UI_HCENTER, T, T, 0, 0, 0, M);
            demorect(box, "Layout( UI_HFILL )", 0.15f, 0, UI_HFILL, T, T, M, 0, M, M);
            demorect(box, "Layout( UI_RIGHT )", 0.25f, 0, UI_RIGHT, T, T, 0, 0, M, M);
        }
        {
            int box = demorect(parent, "Box( UI_COLUMN )\nLayout( UI_FILL )", 0.6f, UI_COLUMN, UI_FILL, S, 0, 0, M, M, M);
            
            demorect(box, "Layout( UI_LEFT )", 0.05f, 0, UI_LEFT, T, T, M, M, 0, M);
            demorect(box, "Layout( UI_HCENTER )", 0.1f, 0, UI_HCENTER, T, T, 0, 0, 0, M);
            demorect(box, "Layout( UI_HFILL )", 0.15f, 0, UI_HFILL, T, T, M, 0, M, M);
            demorect(box, "Layout( UI_RIGHT )", 0.25f, 0, UI_RIGHT, T, T, 0, 0, M, M);
        }
        {
            int box = demorect(parent, "Box( UI_COLUMN )\nLayout( UI_FILL )", 0.6f, UI_COLUMN, UI_FILL, S, 0, 0, M, M, M);
            
            demorect(box, "Layout( UI_LEFT )", 0.05f, 0, UI_LEFT, T, T, M, M, 0, M);
            demorect(box, "Layout( UI_HCENTER )", 0.1f, 0, UI_HCENTER, T, T, 0, 0, 0, M);
            demorect(box, "Layout( UI_HFILL )", 0.15f, 0, UI_HFILL, T, T, M, 0, M, M);
            demorect(box, "Layout( UI_VFILL )", 0.2f, 0, UI_VFILL, T, T, 0, 0, 0, M);
            demorect(box, "Layout( UI_VFILL )", 0.2f, 0, UI_VFILL, T, T, 0, 0, 0, M);
            demorect(box, "Layout( UI_VFILL )", 0.2f, 0, UI_VFILL, T, T, 0, 0, 0, M);
            demorect(box, "Layout( UI_RIGHT )", 0.25f, 0, UI_RIGHT, T, T, 0, 0, M, M);
        }
    }
    
    void fill_wrap_row_box(int box) {
        const int M = 5;
        const int S = 100;
        const int T = 50;
        
        srand(303);
        for (int i = 0; i < 20; ++i) {
            float hue = (float)(rand()%360)/360.0f;
            int width = 10 + (rand()%5)*10;
            
            int u;
            switch(rand()%4) {
                default: break;
                case 0: {
                    u = demorect(box, "Layout( UI_TOP )",
                                 hue, 0, UI_TOP, width, T, M, M, M, M);
                } break;
                case 1: {
                    u = demorect(box, "Layout( UI_VCENTER )",
                                 hue, 0, UI_VCENTER, width, T/2, M, M, M, M);
                } break;
                case 2: {
                    u = demorect(box, "Layout( UI_VFILL )",
                                 hue, 0, UI_VFILL, width, T, M, M, M, M);
                } break;
                case 3: {
                    u = demorect(box, "Layout( UI_DOWN )",
                                 hue, 0, UI_DOWN, width, T/2, M, M, M, M);
                } break;
            }
            
            if (rand()%10 == 0)
                uiSetLayout(u, uiGetLayout(u)|UI_BREAK);
            
        }
        
    }
    
    void fill_wrap_column_box(int box) {
        const int M = 5;
        const int S = 100;
        const int T = 50;
        
        srand(303);
        for (int i = 0; i < 20; ++i) {
            float hue = (float)(rand()%360)/360.0f;
            int height = 10 + (rand()%5)*10;
            
            int u;
            switch(rand()%4) {
                default: break;
                case 0: {
                    u = demorect(box, "Layout( UI_LEFT )",
                                 hue, 0, UI_LEFT, T, height, M, M, M, M);
                } break;
                case 1: {
                    u = demorect(box, "Layout( UI_HCENTER )",
                                 hue, 0, UI_HCENTER, T/2, height, M, M, M, M);
                } break;
                case 2: {
                    u = demorect(box, "Layout( UI_HFILL )",
                                 hue, 0, UI_HFILL, T, height, M, M, M, M);
                } break;
                case 3: {
                    u = demorect(box, "Layout( UI_RIGHT )",
                                 hue, 0, UI_RIGHT, T/2, height, M, M, M, M);
                } break;
            }
            
            if (rand()%10 == 0)
                uiSetLayout(u, uiGetLayout(u)|UI_BREAK);
        }
        
    }
    
    void build_wrapdemo(int parent) {
        int col = uiItem();
        uiInsert(parent, col);
        uiSetBox(col, UI_COLUMN);
        uiSetLayout(col, UI_FILL);
        
        const int M = 5;
        const int S = 100;
        const int T = 50;
        
        int box;
        box = demorect(col, "Box( UI_ROW | UI_WRAP | UI_START )\nLayout( UI_HFILL | UI_TOP )",
                       0.6f, UI_ROW | UI_WRAP | UI_START, UI_TOP, 0, 0, M, M, M, M);
        fill_wrap_row_box(box);
        
        box = demorect(col, "Box( UI_ROW | UI_WRAP | UI_MIDDLE )\nLayout( UI_HFILL | UI_TOP )",
                       0.6f, UI_ROW | UI_WRAP, UI_HFILL | UI_TOP, 0, 0, M, M, M, M);
        fill_wrap_row_box(box);
        
        box = demorect(col, "Box( UI_ROW | UI_WRAP | UI_END )\nLayout( UI_HFILL | UI_TOP )",
                       0.6f, UI_ROW | UI_WRAP | UI_END, UI_HFILL | UI_TOP, 0, 0, M, M, M, M);
        fill_wrap_row_box(box);
        
        box = demorect(col, "Box( UI_ROW | UI_WRAP | UI_JUSTIFY )\nLayout( UI_HFILL | UI_TOP )",
                       0.6f, UI_ROW | UI_WRAP | UI_JUSTIFY, UI_HFILL | UI_TOP, 0, 0, M, M, M, M);
        fill_wrap_row_box(box);
        
        box = demorect(col, "Box( UI_COLUMN | UI_WRAP | UI_START )\nLayout( UI_LEFT | UI_VFILL )",
                       0.6f, UI_COLUMN | UI_WRAP | UI_START, UI_LEFT | UI_VFILL, 0, 0, M, M, M, M);
        fill_wrap_column_box(box);
    }
    
    
    int add_menu_option(int parent, const char *name, int *choice) {
		int opt = UI::RadioBox(-1, name, choice);
        uiInsert(parent, opt);
        uiSetLayout(opt, UI_HFILL|UI_TOP);
        uiSetMargins(opt, 1, 1, 1, 1);
        return opt;
    }
    
    void draw(NVGcontext *vg, float w, float h) {
        bnd_theme.backgroundColor.a = 0.3f;
        bndBackground(vg, 0, 0, w, h);

        uiBeginLayout();
        
        int root = UI::Panel(-1, w, h, 0);
        // position root element
        ((UIData*)uiGetHandle(root))->handler = roothandler;
        uiSetEvents(root, UI_SCROLL|UI_BUTTON0_DOWN);
        uiSetBox(root, UI_COLUMN);
        
        static int choice = 1;
        
        int menu = uiItem();
        uiSetLayout(menu, UI_HFILL|UI_LEFT);
        uiSetBox(menu, UI_ROW);
        uiInsert(root, menu);
        
        int opt_blendish_demo = add_menu_option(menu, "Blendish Demo", &choice);
        int opt_oui_demo = add_menu_option(menu, "OUI Demo", &choice);
        int opt_layouts = add_menu_option(menu, "UI_LAYOUT", &choice);
        int opt_row = add_menu_option(menu, "UI_ROW", &choice);
        int opt_column = add_menu_option(menu, "UI_COLUMN", &choice);
        int opt_wrap = add_menu_option(menu, "UI_WRAP", &choice);
        if (choice < 0)
            choice = opt_blendish_demo;
        
        int content = uiItem();
        uiSetLayout(content, UI_FILL);
        uiInsert(root, content);
        
        if (choice == opt_blendish_demo) {
            int democontent = uiItem();
            uiSetLayout(democontent, UI_FILL);
            uiInsert(content, democontent);
            
            UIData *data = (UIData *)uiAllocHandle(democontent, sizeof(UIData));
            data->handler = 0;
            data->subtype = ST_DEMOSTUFF;
        } else if (choice == opt_oui_demo) {
            int democontent = uiItem();
            uiSetLayout(democontent, UI_TOP);
            uiSetSize(democontent, 250, 0);
            uiInsert(content, democontent);
            
            build_democontent(democontent);
        } else if (choice == opt_layouts) {
            build_layoutdemo(content);
        } else if (choice == opt_row) {
            build_rowdemo(content);
        } else if (choice == opt_column) {
            build_columndemo(content);
        } else if (choice == opt_wrap) {
            build_wrapdemo(content);
        }
        
        uiEndLayout();
        
        UI::DrawUI(0, BND_CORNER_NONE);
        
#if 0
        for (int i = 0; i < uiGetLastItemCount(); ++i) {
            if (uiRecoverItem(i) == -1) {
                UIitem *pitem = uiLastItemPtr(i);
                nvgBeginPath(vg);
                nvgRect(vg,pitem->margins[0],pitem->margins[1],pitem->size[0],pitem->size[1]);
                nvgStrokeWidth(vg, 2);
                nvgStrokeColor(vg, nvgRGBAf(1.0f,0.0f,0.0f,0.5f));
                nvgStroke(vg);
            }
        }
#endif
        
        if (choice == opt_blendish_demo) {
            UIvec2 cursor = uiGetCursor();
            cursor.x -= w/2;
            cursor.y -= h/2;
            if (abs(cursor.x) > (w/3)) {
                bndJoinAreaOverlay(vg, 0, 0, w, h, 0, (cursor.x > 0));
            } else if (abs(cursor.y) > (h/3)) {
                bndJoinAreaOverlay(vg, 0, 0, w, h, 1, (cursor.y > 0));
            }
        }
        
        uiProcess((int)(glfwGetTime()*1000.0));
    }
    
    
void UI::test()
{
    draw(vg, width, height);
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