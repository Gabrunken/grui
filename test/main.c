#include "raylib.h"
#include <raygui.h>
#include <grui.h>
#include <stdio.h>

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetConfigFlags(FLAG_MSAA_4X_HINT);

	InitWindow(800, 600, "Window");
	if (!GRUI_Init()) return 1;

	SetWindowMinSize(300, 200);
	SetTargetFPS(60);

	Vector2 scroll;

	Shader shader = LoadShader(0, "./horizon.fs");

	int winPosLoc = GetShaderLocation(shader, "windowPos");
    int winSizeLoc = GetShaderLocation(shader, "windowSize");
    int monSizeLoc = GetShaderLocation(shader, "monitorSize");

	GRUI_SetShader(shader);

	struct ContainerStyle defaultContainerStyle = {
		3,
		DARKPURPLE,
		GRAY,
		0.4f,
		"NULL"
	};

	while (!WindowShouldClose())
	{
		Vector2 winPos = GetWindowPosition();
        Vector2 winSize = { (float)GetScreenWidth(), (float)GetScreenHeight() };

        int currentMonitor = GetCurrentMonitor();
        Vector2 monSize = { (float)GetMonitorWidth(currentMonitor), (float)GetMonitorHeight(currentMonitor) };

        SetShaderValue(shader, winPosLoc, &winPos, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, winSizeLoc, &winSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, monSizeLoc, &monSize, SHADER_UNIFORM_VEC2);

		BeginDrawing();
		ClearBackground(BLACK);

		GRUI_BeginFrame();

		GRUI_BeginContainer(0.0f, 0.0f, 1.0f, 0.15f, 0.0f, 0.0f, BottomLeft, Row, &scroll, 0.01, &defaultContainerStyle);



		GRUI_EndContainer(&scroll);

		GRUI_EndFrame();

		EndDrawing();
	}

	UnloadShader(shader);

	GRUI_CleanUp();
}
