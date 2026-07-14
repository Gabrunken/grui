#include "raylib.h"
#include <grui.h>
#include <stdlib.h>
#include <stdio.h>
#define DYARRAY_IMPL
#include <dyarray.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#define GRUI_ASSERT(condition) do {if (!condition) exit(1);} while(0)
#define GRUI_WARNING(caller, format, ...) printf("\033[33m" caller " Warning: " format "\n" __VA_OPT__(, __VA_ARGS__) "\033[39m");
#define GRUI_ERROR(caller, format, ...) printf("\033[31m" caller " Warning: " format "\n" __VA_OPT__(, __VA_ARGS__) "\033[39m");

struct UIElement
{
	Rectangle rect;
	Vector2 origin;
};

struct RayGUIStyle
{
	int backgroundColor;
	int borderWidth;
};

struct GRUIContext
{
	bool initialized;
	dyarray containerStack;
	Shader shader;
	struct RayGUIStyle originalStyle;
};

struct GRUIFrameContext
{
	Vector2 mousePosition;
	Vector2 windowSize;
	bool hasBegun;
};

struct Container
{
	struct UIElement uiElement;
	enum ContainerAlignment alignment;
	enum ContainerType type;
	float contentPixelSize; //Represents the current size of the content in this container, in the corresponding axis of the container's type (Row X, Column Y)
	Vector2 scroll;
};

static struct GRUIContext context;
static struct GRUIFrameContext frameContext;

bool GRUI_Init()
{
	if (context.initialized) {GRUI_WARNING("GRUI_Init", "context is already initialized"); return true;}
	GRUI_ASSERT(IsWindowReady());

	if (!DYArrayCreate(&context.containerStack, sizeof(struct Container), 10)) {GRUI_ERROR("GRUI_Init", "failed to create containerStack"); return false;}

	context.originalStyle.backgroundColor = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
	context.originalStyle.borderWidth = GuiGetStyle(DEFAULT, BORDER_WIDTH);

	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(BLANK));
	GuiSetStyle(DEFAULT, BORDER_WIDTH, 0);

	context.initialized = true;
	return true;
}

void GRUI_CleanUp()
{
	if (!context.initialized) {GRUI_WARNING("GRUI_CleanUp", "context is not initialized"); return;}

	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, context.originalStyle.backgroundColor);
	GuiSetStyle(DEFAULT, BORDER_WIDTH, context.originalStyle.borderWidth);

	DYArrayFree(&context.containerStack);
}

void GRUI_BeginFrame()
{
	GRUI_ASSERT(context.initialized && !frameContext.hasBegun);
	frameContext.hasBegun = true;

	frameContext.mousePosition = GetMousePosition();
	frameContext.windowSize = (Vector2){GetScreenWidth(), GetScreenHeight()};
}

void GRUI_EndFrame()
{
	GRUI_ASSERT(frameContext.hasBegun);
	frameContext.hasBegun = false;

	//Clear out any unclosed containers
	context.containerStack.elementCount = 0;
}

void GRUI_SetShader(Shader shader)
{
	GRUI_ASSERT(context.initialized);

	context.shader = shader;
}

Vector2 _GRUI_GetAnchorFromAlignment(enum ContainerAlignment alignment)
{
	switch (alignment)
	{
	case TopLeft:
		return (Vector2){0.0f, 0.0f};
		break;
	case TopRight:
		return (Vector2){1.0f, 0.0f};
		break;
	case BottomLeft:
		return (Vector2){0.0f, 1.0f};
		break;
	case BottomRight:
		return (Vector2){1.0f, 1.0f};
		break;
	default:
		GRUI_ASSERT(false);
		break;
	}
}

//Adjusts the passed ui element to fit the last container in the stack.
//Also stacks up the current element size in the container's contentPixelSize param.
void _GRUI_AdjustRect(struct UIElement* element)
{
	GRUI_ASSERT(frameContext.hasBegun && element);

	if (context.containerStack.elementCount > 0)
	{
		struct Container* parent = DYArrayGetElement(&context.containerStack, context.containerStack.elementCount - 1); //Last element

		element->rect.width *= parent->uiElement.rect.width;
		element->rect.height *= parent->uiElement.rect.height;

		element->origin = _GRUI_GetAnchorFromAlignment(parent->alignment);

		Vector2 originOffset;
		originOffset.x = element->origin.x * element->rect.width;
		originOffset.y = element->origin.y * element->rect.height;

		if (parent->type == Row)
		{
			element->rect.y = element->origin.y * parent->uiElement.rect.height + parent->uiElement.rect.y - originOffset.y;
			element->rect.x = element->origin.x * parent->uiElement.rect.width + parent->uiElement.rect.x - originOffset.x + parent->contentPixelSize;
			parent->contentPixelSize += element->rect.width + 0.008 * parent->uiElement.rect.width /* fixed margin */;
		}

		else
		{
			element->rect.x = element->origin.x * parent->uiElement.rect.width + parent->uiElement.rect.x - originOffset.x;

			//Alignment
			if (element->origin.y == 0.0f)
			{
				element->rect.y = element->origin.y * parent->uiElement.rect.height + parent->uiElement.rect.y - originOffset.y + parent->contentPixelSize;
			}

			else if (element->origin.y == 1.0f)
			{
				element->rect.y = element->origin.y * parent->uiElement.rect.height + parent->uiElement.rect.y - originOffset.y - parent->contentPixelSize;
			}

			parent->contentPixelSize += element->rect.height + 0.008 * parent->uiElement.rect.height /* fixed margin */;
		}

		element->rect.x += parent->scroll.x;
		element->rect.y += parent->scroll.y;

		return;
	}

	//Use window bounds as a generic container
	element->rect.width *= frameContext.windowSize.x;
	element->rect.height *= frameContext.windowSize.y;

	Vector2 originOffset;
	originOffset.x = element->origin.x * element->rect.width;
	element->rect.x = element->rect.x * frameContext.windowSize.x - originOffset.x;

	originOffset.y = element->origin.y * element->rect.height;
	element->rect.y = element->rect.y * frameContext.windowSize.y - originOffset.y;
}

void GRUI_BeginContainer(
    float posX, float posY,
    float width, float height,
    float originX, float originY,
    enum ContainerAlignment alignment, enum ContainerType type,
    const Vector2* scroll,
    float margin,
    const struct ContainerStyle* containerStyle)
{
	GRUI_ASSERT(frameContext.hasBegun);

	struct Container container = {0};
	container.alignment = alignment;
	container.type = type;
	container.scroll = *scroll;

	struct UIElement uiElement;
	container.uiElement.rect.x = posX;
	container.uiElement.rect.y = posY;
	container.uiElement.rect.width = width;
	container.uiElement.rect.height = height;
	container.uiElement.origin.x = originX;
	container.uiElement.origin.y = originY;

	_GRUI_AdjustRect(&container.uiElement);

	DYArrayAddElement(&context.containerStack, &container);

	BeginScissorMode(container.uiElement.rect.x, container.uiElement.rect.y, container.uiElement.rect.width, container.uiElement.rect.height);

	//Lock input outside of this Container
	if (!CheckCollisionPointRec(frameContext.mousePosition, container.uiElement.rect))
    {
        GuiLock();
    }

    else
    {
        GuiUnlock();
    }

	if (containerStyle->fillColor.a != 0)
	{
		if (IsShaderReady(context.shader)) //If the context has any shader
		{
	        BeginShaderMode(context.shader);
	       	DrawRectangleRounded(
				container.uiElement.rect,
				containerStyle->roundness,
				20,
				containerStyle->fillColor);
	        EndShaderMode();
    	}

		else
		{
			DrawRectangleRounded(
				container.uiElement.rect,
				containerStyle->roundness + 0.06f /* or else the outline does not fit right... */,
				20,
				containerStyle->fillColor);
    	}
	}

	if (containerStyle->borderWidth > 0 && containerStyle->borderColor.a != 0)
	{
		DrawRectangleRoundedLines(
			(Rectangle){
				container.uiElement.rect.x + containerStyle->borderWidth,
				container.uiElement.rect.y + containerStyle->borderWidth,
				container.uiElement.rect.width - (containerStyle->borderWidth * 2),
				container.uiElement.rect.height - (containerStyle->borderWidth * 2),
			} /*Make an INNER outline, not OUTER*/,
			containerStyle->roundness,
			20,
			containerStyle->borderWidth,
			containerStyle->borderColor);
	}
}

void GRUI_EndContainer(Vector2* scroll)
{
	GRUI_ASSERT(frameContext.hasBegun);

	if (context.containerStack.elementCount == 0)
	{
		GRUI_ERROR("GRUI_EndContainer", "there are not containers to pop.");
		return;
	}

	struct Container* container = DYArrayGetElement(&context.containerStack, context.containerStack.elementCount - 1);

	Rectangle view;
	Rectangle content;
	if (container->type == Row) {content.width = container->contentPixelSize - 20; content.height = container->uiElement.rect.height - 20;}
	else {content.height = container->contentPixelSize - 20; content.width = container->uiElement.rect.width - 20;}
	content.x = container->uiElement.rect.x;
	content.y = container->uiElement.rect.y;

	GuiScrollPanel(container->uiElement.rect, NULL, content, scroll, &view);

	EndScissorMode();
	GuiUnlock();

	DYArrayRemoveElementSP(&context.containerStack, context.containerStack.elementCount - 1);
}

bool _GRUI_IsElementCullable(Rectangle elementRect)
{
	GRUI_ASSERT(frameContext.hasBegun);

	if (context.containerStack.elementCount > 0)
	{
		struct Container* container = DYArrayGetElement(&context.containerStack, context.containerStack.elementCount - 1);
		if (CheckCollisionRecs(container->uiElement.rect, elementRect))
		{
			return false;
		}
	}

	else
	{
		if (CheckCollisionRecs((Rectangle){0,0,frameContext.windowSize.x,frameContext.windowSize.y}, elementRect))
		{
			return false;
		}
	}

	return true;
}

//======================= Primitives =======================\\

bool GRUI_Button(
    float posX, float posY,
    float width, float height,
    float originX, float originY,
    const char* text)
{
	GRUI_ASSERT(frameContext.hasBegun);

	if (!text)
	{
		GRUI_WARNING("GRUI_Button", "text is NULL");
	}

	struct UIElement uiElement = {(Rectangle){posX, posY, width, height}, (Vector2){originX, originY}};
	_GRUI_AdjustRect(&uiElement);

	if (_GRUI_IsElementCullable(uiElement.rect))
		return false;

	return GuiButton(uiElement.rect, text);
}

void GRUI_GroupBox(
    float posX, float posY,
    float width, float height,
    float originX, float originY,
    const char* title
)
{
	GRUI_ASSERT(frameContext.hasBegun);

	struct UIElement uiElement = {(Rectangle){posX, posY, width, height}, (Vector2){originX, originY}};
	_GRUI_AdjustRect(&uiElement);

	if (_GRUI_IsElementCullable(uiElement.rect))
		return;

	GuiGroupBox(uiElement.rect, title);
}

//==============================================\\
