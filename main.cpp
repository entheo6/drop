#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <thread>
#include <atomic>
#include <cmath>
#include <Windows.h>

enum LOOK_DIR
{
	UP,
	DOWN
};

struct vec2
{
	int x, y;

	vec2() {}
	vec2(int a, int b) { x = a; y = b; }
};

struct GridMovement
{
	vec2 position;
	bool fire;

	GridMovement()
		:position(vec2(0, 0)), fire(false) {}
	GridMovement(vec2 p, bool f)
		:position(p), fire(f) {}
};

//std::vector<GridMovement[]> charPatterns;

bool bAbort = false;

std::atomic<bool> moving = false,
firing = false;

static const int NUM_KEYS = 9;

const int COMMON_X = 680,
INVENTORY_Y = 710,
STORAGE_Y = 300,
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
THROTTLE_COLUMN = 120,
THROTTLE_DEFAULT = 50,
LOOK_DELTA = 192,
GRID_MOVE_DURATION = 750,
ARROW_HOLD_DURATION = 200;

const int COORD_MAP_Y[] = { HOTBAR_Y,  INVENTORY_Y, (INVENTORY_Y - INCREMENT_Y), (INVENTORY_Y - (INCREMENT_Y * 2)) };

vec2 currentGridPos = vec2(0, 0);

void init();
void pollVisibility();
bool gameFocused();
void escape();
void click(int, int);
void rightClick(int, int);
void shiftClick(int, int);
void holdRightClick(int);
void craftRockets();
void moveRockets();
void craftBlocks();
void craftConcrete();
void look(LOOK_DIR);
void holdKey(WORD, int);
void gridMove(vec2, bool);
void rapidFire(int);

enum KEYS
{
	ESCAPE,
	CONTROL,
	BACKSPACE,
	MOUSE_LEFT,
	MOUSE_RIGHT,
	F9,
	NUMPAD1,
	NUMPAD2,
	NUMPAD3
};

int main()
{
	init();

	bool keyDown[NUM_KEYS];
	const int DROP_X = 1550;
	POINT downPos;

	// Program started indicator
	Beep(700, 200);

	/*
		Poll key combos
	*/

	while (true)
	{
		Sleep(10);

		// Complementary keys
		if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) && !keyDown[CONTROL])
			keyDown[CONTROL] = true;

		// Control + backspace (abort)
		if (keyDown[CONTROL] && GetAsyncKeyState(VK_BACK))
			break;	

		if (gameFocused())
		{
			// Control click (drop)
			if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && !keyDown[MOUSE_LEFT])
			{
				if (keyDown[CONTROL])
				{
					GetCursorPos(&downPos);
					Sleep(75);
					click(DROP_X, downPos.y);
					SetCursorPos(downPos.x, downPos.y);
					keyDown[MOUSE_LEFT] = true;
				}
			}

			// Numpad 1 (craft rockets)
			if ((GetAsyncKeyState(VK_NUMPAD1) & 0x8000) && !keyDown[NUMPAD1])
			{
				craftRockets();
				keyDown[NUMPAD1] = true;
			}

			// Numpad 2 (craft blocks)
			if ((GetAsyncKeyState(VK_NUMPAD2) & 0x8000) && !keyDown[NUMPAD2])
			{
				craftBlocks();
				keyDown[NUMPAD2] = true;
			}

			// Numpad 3 (craft concrete)
			if ((GetAsyncKeyState(VK_NUMPAD3) & 0x8000) && !keyDown[NUMPAD3])
			{
				craftConcrete();
				keyDown[NUMPAD3] = true;
			}
		}

		// Reset keyDown
		if (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000) && keyDown[ESCAPE])
			keyDown[ESCAPE] = false;

		if (!(GetAsyncKeyState(VK_LCONTROL) & 0x8000) && keyDown[CONTROL])
			keyDown[CONTROL] = false;

		if (!(GetAsyncKeyState(VK_BACK) & 0x8000) && keyDown[BACKSPACE])
			keyDown[BACKSPACE] = false;

		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000) && keyDown[MOUSE_LEFT])
			keyDown[MOUSE_LEFT] = false;

		if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000) && keyDown[MOUSE_RIGHT])
			keyDown[MOUSE_RIGHT] = false;

		if (!(GetAsyncKeyState(VK_F9) & 0x8000) && keyDown[F9])
			keyDown[F9] = false;

		if (!(GetAsyncKeyState(VK_NUMPAD1) & 0x8000) && keyDown[NUMPAD1])
			keyDown[NUMPAD1] = false;

		if (!(GetAsyncKeyState(VK_NUMPAD2) & 0x8000) && keyDown[NUMPAD2])
			keyDown[NUMPAD2] = false;

		if (!(GetAsyncKeyState(VK_NUMPAD3) & 0x8000) && keyDown[NUMPAD3])
			keyDown[NUMPAD3] = false;
	}

	// Program terminated indicator
	Beep(620, 200);
	std::cout << "\n  End\n";

	// Debug
	//while (true)
	//{
	//	GetCursorPos(&downPos);
	//	system("cls");
	//	std::cout << "x: " << downPos.x << ", y: " << downPos.y;
	//	Sleep(50);
	//}

	return EXIT_SUCCESS;
}

void init()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	system("title drop & color 04");

	std::string consoleText = "\n\n  Running!\n\n";

	consoleText += "  [F9] : Toggle visibility\n\n";

	consoleText += "  [ctrl + left click] : Drop inventory item\n\n";

	consoleText += "  [numpad 1] : Craft rockets\n";
	consoleText += "  [numpad 2] : Craft blocks\n";
	consoleText += "  [numpad 3] : Craft concrete\n\n";

	consoleText += "  [esc] : Abort current operation\n";
	consoleText += "  [ctrl + backspace] : Terminate\n";

	std::cout << consoleText;

	std::thread visibilityThread(pollVisibility);
	visibilityThread.detach();
}

void pollVisibility()
{
	bool keyDown = false,
		visible = false;

	while (!bAbort)
	{
		if ((GetAsyncKeyState(VK_F9) & 0x8000) && !keyDown)
		{
			visible = !visible;
			ShowWindow(GetConsoleWindow(), (visible ? SW_SHOW : SW_HIDE));
			keyDown = true;
		}

		else if (!(GetAsyncKeyState(VK_F9) & 0x8000) && keyDown)
			keyDown = false;

		Sleep(THROTTLE_DEFAULT);
	}
}

bool gameFocused()
{
	const int MAX_LENGTH = 256;
	char title[MAX_LENGTH];

	HWND hwnd = GetForegroundWindow();

	int length = GetWindowTextA(hwnd, title, MAX_LENGTH);

	if (length)
	{
		std::string str(title);

		for (int i = 0; i < str.length(); ++i)
			str[i] = std::toupper(str[i]);

		if (str.find("MINECRAFT") != std::string::npos)
			return true;
	}

	return false;
}

void escape()
{
	INPUT ip = {};
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = VK_ESCAPE;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void click(int x, int y)
{
	INPUT input[2];

	input[0].type = INPUT_MOUSE;
	input[0].mi.mouseData = 0;
	input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	input[0].mi.time = 0;
	input[0].mi.dwExtraInfo = 0;
	input[0].mi.dx = x * (65535 / GetSystemMetrics(SM_CXSCREEN));
	input[0].mi.dy = y * (65535 / GetSystemMetrics(SM_CYSCREEN));

	input[1].type = INPUT_MOUSE;
	input[1].mi.mouseData = 0;
	input[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	input[1].mi.time = 0;
	input[1].mi.dwExtraInfo = 0;

	SendInput(2, input, sizeof(INPUT));

	input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(1, &input[1], sizeof(INPUT));
}

void rightClick(int x, int y)
{
	INPUT Inputs[3] = { 0 };

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = x * (65535 / GetSystemMetrics(SM_CXSCREEN));
	Inputs[0].mi.dy = y * (65535 / GetSystemMetrics(SM_CYSCREEN));
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

	Inputs[2].type = INPUT_MOUSE;
	Inputs[2].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

	SendInput(3, Inputs, sizeof(INPUT));
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

void holdRightClick(int duration)
{
	INPUT input[2] = {};

	input[0].type = INPUT_MOUSE;
	input[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

	input[1].type = INPUT_MOUSE;
	input[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

	SendInput(1, input, sizeof(INPUT));
	Sleep(duration);
	SendInput(1, input + 1, sizeof(INPUT));
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
			shiftClick((COMMON_X + ((i)*INCREMENT_X)), COORD_MAP_Y[j]);
		}

		Sleep(THROTTLE_COLUMN + rand() % 10 - 5);
		shiftClick(CRAFT_X, CRAFT_Y);
	}
}

void craftBlocks()
{
	Sleep(SLEEP_DURATION_INIT);

	for (int i = 0; i < NUM_COLUMNS; ++i)
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			break;

		for (int j = 1; j < NUM_ROWS; ++j)
		{
			Sleep(THROTTLE_CELL + rand() % 10 - 5);
			shiftClick((COMMON_X + ((i)*INCREMENT_X)), COORD_MAP_Y[j]);
		}

		Sleep(THROTTLE_COLUMN + rand() % 10 - 5);

		if (i == 2 || i == 5 || i == 8)
		{
			Sleep(THROTTLE_COLUMN + rand() % 50 - 25);
			shiftClick(CRAFT_X, CRAFT_Y);
		}
	}
}

void craftConcrete()
{
	Sleep(SLEEP_DURATION_INIT);

	for (int i = 1; i < NUM_ROWS; ++i)
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			break;

		for (int j = 0; j < NUM_COLUMNS; ++j)
		{
			Sleep(THROTTLE_CELL + rand() % 10 - 5);
			shiftClick((COMMON_X + ((j)*INCREMENT_X)), COORD_MAP_Y[i]);
		}

		Sleep(THROTTLE_COLUMN + rand() % 100 - 25);
		shiftClick(CRAFT_X, CRAFT_Y);
	}
}

void look(LOOK_DIR direction)
{
	POINT currentPos;

	GetCursorPos(&currentPos);

	for (int i = 0; i < (direction == UP ? 10 : 4); ++i)
	{
		SetCursorPos(currentPos.x, currentPos.y - (direction == UP ? LOOK_DELTA : LOOK_DELTA * -1));
		Sleep(10);
	}
}

void holdKey(WORD keyCode, int duration)
{
	moving = true;

	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = keyCode;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(duration);

	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	moving = false;
}

void holdKeys(WORD keyCode1, WORD keyCode2, DWORD duration)
{
	if (!moving)
	{
		moving = true;

		INPUT inputs[4] = {}; // Array to hold the input events

		// 1. Press Key 1
		inputs[0].type = INPUT_KEYBOARD;
		inputs[0].ki.wVk = keyCode1;
		inputs[0].ki.dwFlags = 0;

		// 2. Press Key 2
		inputs[1].type = INPUT_KEYBOARD;
		inputs[1].ki.wVk = keyCode2;
		inputs[1].ki.dwFlags = 0;

		// 3. Release Key 1 (after duration)
		inputs[2].type = INPUT_KEYBOARD;
		inputs[2].ki.wVk = keyCode1;
		inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

		// 4. Release Key 2
		inputs[3].type = INPUT_KEYBOARD;
		inputs[3].ki.wVk = keyCode2;
		inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

		UINT uSent = SendInput(4, inputs, sizeof(INPUT)); // Send all inputs at once

		if (uSent != 4)
		{
			std::cerr << "Error sending input: " << GetLastError() << std::endl;
		}

		else
		{
			Sleep(duration); // Hold keys for specified duration (milliseconds)
			SendInput(2, &inputs[2], sizeof(INPUT)); // Release both keys
		}

		moving = false;
	}
}

void gridMove(vec2 pos, bool fire)
{
	/*
	*		Move player to 3x3 grid position (1-9)
	*		2nd arg indicates whether to fire arrows while moving
	*/

	currentGridPos = vec2(0, 0); //debug

	if (GetAsyncKeyState(VK_ESCAPE))
		system("taskkill /f /im drop.exe");

	vec2 deltaPos = vec2(currentGridPos.x + pos.x, currentGridPos.y + pos.y);

	look(UP);

	// Basic diagonal movement
	if (abs(deltaPos.x == 1) && abs(deltaPos.y == 1))
	{
		if (fire)
			std::thread fireThread(holdRightClick, GRID_MOVE_DURATION);

		holdKeys((deltaPos.x > 0 ? VkKeyScan('d') : VkKeyScan('a')), (deltaPos.y > 0 ? VkKeyScan('s') : VkKeyScan('w')), GRID_MOVE_DURATION);
	}

	// Horizontal movement
	if ((abs(deltaPos.x) > 0 && abs(deltaPos.y) == 0) || (abs(deltaPos.y) > 0 && abs(deltaPos.x) == 0))
	{
		// X-axis
		for (int i = 0; i < abs(deltaPos.x); ++i)
		{
			if (fire)
			{
				std::thread fireThread(rapidFire, GRID_MOVE_DURATION);
				holdKey(deltaPos.x > 0 ? VkKeyScan('d') : VkKeyScan('a'), GRID_MOVE_DURATION);
				fireThread.join();
			}

			else
				holdKey(deltaPos.x > 0 ? VkKeyScan('d') : VkKeyScan('a'), GRID_MOVE_DURATION * 0.5f);
		}

		// Y-axis
		for (int i = 0; i < abs(deltaPos.y); ++i)
		{
			if (fire)
			{
				std::thread fireThread(rapidFire, GRID_MOVE_DURATION);
				holdKey(deltaPos.y > 0 ? VkKeyScan('s') : VkKeyScan('w'), (GRID_MOVE_DURATION + 100));
				fireThread.join();
			}

			else
				holdKey(deltaPos.y > 0 ? VkKeyScan('s') : VkKeyScan('w'), (GRID_MOVE_DURATION + 100) * 0.5f);
		}
	}

	currentGridPos = pos;
}

void rapidFire(int duration)
{
	for (int i = 0; i < (duration / ARROW_HOLD_DURATION); ++i)
	{
		holdRightClick(ARROW_HOLD_DURATION);
		Sleep(50);
	}
}