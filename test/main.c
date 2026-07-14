#include "raylib.h"
#include <raygui.h>
#include <grui.h>
#include <stdio.h>

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(800, 600, "Window");
	if (!GRUI_Init()) return 1;

	SetWindowMinSize(300, 200);
	SetTargetFPS(60);

	Vector2 scroll;

	Shader shader = LoadShader(0, "./horizon.fs");

	GRUI_SetShader(shader);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

		GRUI_BeginFrame();

		GRUI_BeginContainer(0.0f, 0.0f, 1.0f, 0.15f, 0.0f, 0.0f, BottomLeft, Row, &scroll);

		GRUI_GroupBox(0.0f, 0.0f, 0.496f, 0.95f, 0.0f, 0.0f, "Group1");
		GRUI_GroupBox(0.0f, 0.0f, 0.496f, 0.95f, 0.0f, 0.0f, "Group2");

		GRUI_EndContainer(&scroll);

		GRUI_EndFrame();

		EndDrawing();
	}

	UnloadShader(shader);

	GRUI_CleanUp();
}
