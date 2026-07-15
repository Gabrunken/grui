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
    int timeLoc = GetShaderLocation(shader, "time");

	GRUI_SetShader(shader);

	struct ContainerStyle defaultContainerStyle = {
		3,
		LIGHTGRAY,
		GRAY,
		0.4f,
		"NULL"
	};

	Color backgroundColor = BLACK;
	bool checkBox = false;
	int scrollIndex, activeIndex;

	bool exitWindow = false;

	const char* elements[] =
	{
		"Prova",
		"Ciao",
		"Oggetto",
		"Gatto",
		"Player",
		"Nemico"
	};

	while (!exitWindow)
	{
		if (WindowShouldClose()) break;

		Vector2 winPos = GetWindowPosition();
        Vector2 winSize = { (float)GetScreenWidth(), (float)GetScreenHeight() };

        int currentMonitor = GetCurrentMonitor();
        Vector2 monSize = { (float)GetMonitorWidth(currentMonitor), (float)GetMonitorHeight(currentMonitor) };

        float time = GetTime();

        SetShaderValue(shader, winPosLoc, &winPos, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, winSizeLoc, &winSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, monSizeLoc, &monSize, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);

		BeginDrawing();
		ClearBackground(backgroundColor);

		GRUI_BeginFrame();

		GRUI_BeginContainer(0.0f, 0.0f, 1.0f, 0.15f, 0.0f, 0.0f, BottomLeft, Row, &scroll, 0.2f, 0.01f, &defaultContainerStyle, false);

		if (GRUI_Button(0, 0, 0.1, 0.2, 0, 0, "Button", false))
		{
			printf("%d\n", activeIndex);
		}

		GRUI_Label(0, 0, 0.2, 0.2, 0, 0, "Questa è una label");
		GRUI_CheckBox(0, 0, 0.2, 0.5, 0, 0, "Box", &exitWindow, true);

		GRUI_ColorPicker(0, 0, 0.075f, 0.65f, 0, 0, &backgroundColor, true);

		GRUI_SelectableList(0, 0, 0.0f, 0.5f, 0, 0, elements, 6, &scrollIndex, &activeIndex, true);
		GRUI_SelectableList(0, 0, 0.0f, 0.5f, 0, 0, elements, 6, &scrollIndex, &activeIndex, true);
		GRUI_SelectableList(0, 0, 0.0f, 0.5f, 0, 0, elements, 6, &scrollIndex, &activeIndex, true);
		GRUI_SelectableList(0, 0, 0.0f, 0.5f, 0, 0, elements, 6, &scrollIndex, &activeIndex, true);

		GRUI_ColorPicker(0, 0, 0.075f, 0.65f, 0, 0, &backgroundColor, true);

		GRUI_EndContainer(&scroll);

		GRUI_EndFrame();

		EndDrawing();
	}

	UnloadShader(shader);
	CloseWindow();

	GRUI_CleanUp();
}
