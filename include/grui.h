#ifndef GRUI_H_
#define GRUI_H_
#include <raylib.h>
#include <stdint.h>

#define GRUI_NULL_SHADER (Shader){0}

/*Gabro's Raylib User Interface*/
/*
 * Immediate mode UI library made to be integrated with Raylib.
 * The scope of this library is to have a simple and no-bloat API,
 * useful for both quick prototyping and full applications.
 */

enum ContainerAlignment
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

enum ContainerType
{
	Row,
	Column
};

struct ContainerStyle
{
    int borderWidth;
    Color borderColor;
    Color fillColor;
    float roundness; //0 to 1
    const char* title;
};

/*
 * @brief Initializes the library.
 * You must call this before calling any library function.
 * GRUI initialization does require previous raylib setup.
 * @return True on success, False on failure.
 */
bool GRUI_Init();

/*
 * @brief Cleans up the previously initialized library.
 * Call this once you're done with GRUI.
 */
void GRUI_CleanUp();

/*
 * todo doc
 */
void GRUI_BeginFrame();

/*
 * todo doc
 */
void GRUI_EndFrame();

/*
 * todo doc
 */
void GRUI_BeginContainer(
    float posX, float posY,
    float width, float height,
    float originX, float originY,
    enum ContainerAlignment alignment, enum ContainerType type,
    const Vector2* scroll,
    float outerMargin,
    float elementMargin,
    const struct ContainerStyle* containerStyle,
    bool squareAspectRatio);

/*
 * todo doc
 */
void GRUI_EndContainer(Vector2* scroll);

/*
 * todo doc (only affects GRUI drawn elements, not raygui's)
 * set shader to NULL_SHADER if you want to use the default shader
 */
void GRUI_SetShader(Shader shader);

/*
 * @brief Creates a button.
 * Renders and handles input for a simple text button.
 * @return True if clicked, False if not.
 */
bool GRUI_Button(
    float posX, float posY,
    float width, float height,
    float originX, float originY,
    const char* text,
    bool squareAspectRatio
);

bool GRUI_IconButton(
    float posX, float posY,
    float width, float height,
    float originX, float originY,
    Texture2D icon,
    float iconScale,
    Color idleColor,
    Color hoverColor,
    bool squareAspectRatio
);

/*
 * todo doc
 */
void GRUI_ColorPicker(
    float posX, float posY,
    float width, float height,
    float originX, float originY,
    Color* outColor,
    bool squareAspectRatio
);

/*
 * This box's text is meant to be used for single-word, short texts,
 * since it does not expand, but shrinks the font size to fit the rect.
 * Consider using a label combined with a Container to make a CheckBox + Label combo.
 */
void GRUI_CheckBox(
	float posX, float posY,
    float width, float height,
    float originX, float originY,
    const char *text,
    bool* outBool,
    bool squareAspectRatio
);

void GRUI_SelectableList(
	float posX, float posY,
    float width, float height,
    float originX, float originY,
    const char **elements,
    size_t elementCount,
    int* outScrollIndex,
    int* outActive,
    bool squareAspectRatio
);

void GRUI_Label(
    float posX, float posY,
    float width, float height,
    float originX, float originY,
    const char *text
);

#endif
