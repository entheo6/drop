#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

struct vec2
{
	int x, y;

	vec2() {}
	vec2(int a, int b) { x = a; y = b; }
};

const int COMMON_X = 680,
			INVENTORY_Y = 710,
			CHEST_Y = 300,
			HOTBAR_Y = 800,
			CRAFT_X = 1135,
			CRAFT_Y = 370,
			INCREMENT_X = 70,
			INCREMENT_Y = 70,
			NUM_COLUMNS = 9,
			NUM_ROWS = 4,
			SLEEP_DURATION_INIT = 300,
			THROTTLE_CELL = 50,
			THROTTLE_COLUMN = 120;

const int COORD_MAP_Y[] = { HOTBAR_Y,  INVENTORY_Y, (INVENTORY_Y - INCREMENT_Y), (INVENTORY_Y - (INCREMENT_Y * 2))};

void escape();
void click(int, int);
void rightClick(int, int);
void shiftClick(int, int);
void craftRockets();
void moveRockets();

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	system("color 04 & echo Running!");

	bool controlDown = false,
		altDown = false,
		mouseDownL = false,
		mouseDownR = false;

	const int DROP_X = 1550;
	POINT downPos;

	Beep(700, 200);

	while (true)
	{
		Sleep(10);

		if (GetAsyncKeyState(VK_BACK))
			break;

		if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) && !controlDown)
			controlDown = true;
		
		else if (!(GetAsyncKeyState(VK_LCONTROL) & 0x8000) && controlDown)
			controlDown = false;

		if ((GetAsyncKeyState(VK_MENU) & 0x8000) && !altDown)
			altDown = true;

		else if (!(GetAsyncKeyState(VK_MENU) & 0x8000) && altDown)
			altDown = false;
			
		if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && !mouseDownL)
		{
			if (controlDown)	// control click - drop
			{
				mouseDownL = true;
				GetCursorPos(&downPos);
				Sleep(75);
				click(DROP_X, downPos.y);
				SetCursorPos(downPos.x, downPos.y);
			}
		}

		else if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000) && mouseDownL)
			mouseDownL = false;

		if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) && !mouseDownR)
		{
			mouseDownR = true;

			if (controlDown)	// control right click - craft
				craftRockets();

			else if (altDown)	// alt right click - move
				moveRockets();
		}

		else if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000) && mouseDownR)
			mouseDownR = false;
	}

	Beep(620, 200);

	std::cout << "End\n";

	//while (true)	// debug
	//{
	//	GetCursorPos(&downPos);
	//	system("cls");
	//	std::cout << "x: " << downPos.x << ", y: " << downPos.y;
	//	Sleep(50);
	//}

	return EXIT_SUCCESS;
}

void escape()
{
	INPUT ip = {};
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	// Press the Escape key
	ip.ki.wVk = VK_ESCAPE; // Virtual-key code for Escape
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	// Release the Escape key
	ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
	SendInput(1, &ip, sizeof(INPUT));
}

void click(int x, int y)
{
	INPUT input[2];

	// Mouse move event
	input[0].type = INPUT_MOUSE;
	input[0].mi.mouseData = 0;
	input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	input[0].mi.time = 0;
	input[0].mi.dwExtraInfo = 0;
	input[0].mi.dx = x * (65535 / GetSystemMetrics(SM_CXSCREEN));
	input[0].mi.dy = y * (65535 / GetSystemMetrics(SM_CYSCREEN));

	// Mouse down event
	input[1].type = INPUT_MOUSE;
	input[1].mi.mouseData = 0;
	input[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	input[1].mi.time = 0;
	input[1].mi.dwExtraInfo = 0;

	// Send both move and click down event
	SendInput(2, input, sizeof(INPUT));

	// Mouse up event
	input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	// Send mouse up event
	SendInput(1, &input[1], sizeof(INPUT));
}

void rightClick(int x, int y)
{
	INPUT Inputs[3] = { 0 }; // Array to hold the input events
	// Move the mouse
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = x * (65535 / GetSystemMetrics(SM_CXSCREEN)); // Normalize X coordinate
	Inputs[0].mi.dy = y * (65535 / GetSystemMetrics(SM_CYSCREEN)); // Normalize Y coordinate
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	// Simulate right button down
	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

	// Simulate right button up
	Inputs[2].type = INPUT_MOUSE;
	Inputs[2].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

	SendInput(3, Inputs, sizeof(INPUT)); // Send the sequence of events
}

void shiftClick(int x, int y)
{
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = VK_SHIFT;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	click(x, y);

	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void craftRockets()
{
	Sleep(SLEEP_DURATION_INIT);

	for (int i = 0; i < NUM_COLUMNS; ++i)
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			break;

		for (int j = 0; j < NUM_ROWS; ++j)
		{
			Sleep(THROTTLE_CELL + rand() % 10 - 5);
			shiftClick((COMMON_X + ((i) * INCREMENT_X)), COORD_MAP_Y[j]);
		}

		Sleep(THROTTLE_COLUMN + rand() % 10 - 5);
		shiftClick(CRAFT_X, CRAFT_Y);
	}
}

void moveRockets()
{
	Sleep(SLEEP_DURATION_INIT);

	for (int i = 0; i < NUM_COLUMNS; ++i)
	{
		for (int j = 0; j < NUM_ROWS - 1; ++j)
		{
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
				break;

			Sleep(THROTTLE_CELL);
			shiftClick((COMMON_X + ((i)*INCREMENT_X)), COORD_MAP_Y[j]);
		}

		Sleep(THROTTLE_COLUMN);
	}
}