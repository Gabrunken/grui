#ifndef GRUI_H_
#define GRUI_H_
#include <raylib.h>

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

/*
 * @brief Initializes the library.
 * You must call this before calling any library function.
 * GRUI initialization does not require previous raylib setup,
 * but any other function, besides the clean up, does require raylib
 * to be initialized with InitWindow.
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
    const Vector2* scroll);

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
    const char* text);

/*
 * todo doc
 */
void GRUI_GroupBox(
    float posX, float posY,
    float width, float height,
    float originX, float originY,
    const char* title
);

#endif
