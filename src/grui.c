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

struct GRUIContext
{
	bool initialized;
	dyarray containerStack;
	Shader shader;
};

struct Container
{
	struct UIElement uiElement;
	enum ContainerAlignment alignment;
	enum ContainerType type;
	dyarray elements; //Contains Rectangles
	float contentPixelSize;
	Vector2 scroll;
};

static struct GRUIContext context;

bool GRUI_Init()
{
	if (context.initialized) {GRUI_WARNING("GRUI_Init", "context is already initialized"); return true;}

	if (!DYArrayCreate(&context.containerStack, sizeof(struct Container), 10)) {GRUI_ERROR("GRUI_Init", "failed to create containerStack"); return false;}

	context.initialized = true;
	return true;
}

void GRUI_CleanUp()
{
	if (!context.initialized) {GRUI_WARNING("GRUI_CleanUp", "context is not initialized"); return;}

	for (size_t i = 0; i < context.containerStack.elementCount; i++)
	{
		struct Container* container = DYArrayGetElement(&context.containerStack, i);
		DYArrayFree(&container->elements);
	}

	DYArrayFree(&context.containerStack);
}

void GRUI_SetShader(Shader shader)
{
	GRUI_ASSERT(context.initialized && IsWindowReady());

	context.shader = shader;
}

Vector2 _GRUI_GetAnchorFromAlignment(enum ContainerAlignment alignment)
{
	GRUI_ASSERT(context.initialized && IsWindowReady());

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
	GRUI_ASSERT(context.initialized && IsWindowReady());

	GRUI_ASSERT(element);

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
	element->rect.width *= GetScreenWidth();
	element->rect.height *= GetScreenHeight();

	Vector2 originOffset;
	originOffset.x = element->origin.x * element->rect.width;
	element->rect.x = element->rect.x * GetScreenWidth() - originOffset.x;

	originOffset.y = element->origin.y * element->rect.height;
	element->rect.y = element->rect.y * GetScreenHeight() - originOffset.y;
}

void GRUI_BeginContainer(struct UIElement* format, enum ContainerAlignment alignment, enum ContainerType type, const Vector2* scroll)
{
	GRUI_ASSERT(context.initialized && IsWindowReady());

	if (!format)
	{
		GRUI_WARNING("GRUI_BeginContainer", "format is NULL");
		return;
	}

	struct Container container = {0};
	container.alignment = alignment;
	container.type = type;
	container.scroll = *scroll;

	_GRUI_AdjustRect(format);

	memcpy(&container.uiElement, format, sizeof(struct UIElement));
	DYArrayCreate(&container.elements, sizeof(struct Rectangle), 10);
	DYArrayAddElement(&context.containerStack, &container);

	BeginScissorMode(format->rect.x, format->rect.y, format->rect.width, format->rect.height);

	if (IsShaderReady(context.shader)) //If the context has any shader
	{
        BeginShaderMode(context.shader);
        	DrawRectangleRec(container.uiElement.rect, WHITE);
        EndShaderMode();
        return;
	}

	DrawRectangleRec(container.uiElement.rect, WHITE); //Draw with default shader
}

void GRUI_EndContainer(Vector2* scroll)
{
	GRUI_ASSERT(context.initialized && IsWindowReady());

	if (context.containerStack.elementCount == 0)
	{
		GRUI_ERROR("GRUI_EndContainer", "there are not containers to pop.");
		return;
	}

	struct Container* container = DYArrayGetElement(&context.containerStack, context.containerStack.elementCount - 1);

	Rectangle view;
	Rectangle content;
	if (container->type == Row) {content.width = container->contentPixelSize; content.height = container->uiElement.rect.height;}
	else {content.height = container->contentPixelSize; content.width = container->uiElement.rect.width - 20;}
	content.x = container->uiElement.rect.x;
	content.y = container->uiElement.rect.y;

	int originalBg = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
	int originalBorderWidth = GuiGetStyle(DEFAULT, BORDER_WIDTH);
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(BLANK));
	GuiSetStyle(DEFAULT, BORDER_WIDTH, 0);

	GuiScrollPanel(container->uiElement.rect, NULL, content, scroll, &view);

	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, originalBg);
	GuiSetStyle(DEFAULT, BORDER_WIDTH, 0);

	EndScissorMode();

	DYArrayFree(&container->elements);
	DYArrayRemoveElementSP(&context.containerStack, context.containerStack.elementCount - 1);
}

bool GRUI_Button(struct UIElement* uiElement, const char *text)
{
	GRUI_ASSERT(context.initialized && IsWindowReady());

	if (!text)
	{
		GRUI_WARNING("GRUI_Button", "text is NULL");
		return false;
	}

	if (!uiElement)
	{
		GRUI_WARNING("GRUI_Button", "uiElement is NULL");
		return false;
	}

	_GRUI_AdjustRect(uiElement);

	return GuiButton(uiElement->rect, text);
}
