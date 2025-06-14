#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <thread>
#include <atomic>
#include <cmath>
#include <unordered_map>
#include <signal.h>

#include <Windows.h>
#include <shellapi.h>
#include <mmsystem.h>

#include "vec2.h"
#include "resource.h"
#include "custom_actions.h"
#include "persistence.h"
#include "printer.h"
#include "sound.h"

#pragma comment(lib, "winmm.lib")

enum LOOK_DIR
{
	UP,
	DOWN
};

enum FOCUS_TYPE
{
	GAME,
	CONSOLE
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

bool soundEnabled = true,
borderEnabled = false,
startHidden = false;

Persistence persistence;

Sound sound(&soundEnabled);

CustomActions customActions;

Printer printer(&customActions);


bool config[] = { soundEnabled, borderEnabled, startHidden };

std::atomic<bool> moving = false,
				  firing = false,
				  bAbort = false,
				  windowVisible = true;

static const int NUM_KEYS = 15;

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
			ARROW_HOLD_DURATION = 200,
			BASE_WINDOW_HEIGHT = 480,
			WINDOW_HEIGHT_ADJUST = 30,
			WINDOW_HEIGHT_INCREMENT = 10,
			WINDOW_ALPHA = 240,
			PAUSE_DURATION = 1500;

int windowWidth = 980,
	windowHeight = BASE_WINDOW_HEIGHT;

const int COORD_MAP_Y[] = { HOTBAR_Y,  INVENTORY_Y, (INVENTORY_Y - INCREMENT_Y), (INVENTORY_Y - (INCREMENT_Y * 2)) };

const char* instanceId = "696E7374616E63652030";

HANDLE mutex;

vec2 currentGridPos = vec2(0, 0);

void sigint(int);
void removeScrollbar();
void toggleBorder(bool);
void hideCursor();
void load();
void save();
bool initWindow();
void playInitSound();
void resizeWindow();
void resizeWindow(int, int);
void scrollUp();
void amendMainText(std::string);
void pollVisibility();
bool focused(FOCUS_TYPE);
void escape();
void click(int, int);
void rightClick(int, int);
void shiftClick(int, int);
void holdRightClick(int);
void craftRockets();
void craftBlocks();
void craftConcrete();
void look(LOOK_DIR);
void holdKey(WORD, int);
void gridMove(vec2, bool);
void rapidFire(int);
void up(int count) { for (int i = 0; i < count; ++i) std::cout << "\x1b[A"; }

std::vector<unsigned int> virtualKeys = {
											VK_ESCAPE,
											VK_LCONTROL,
											VK_BACK,
											VK_F9,
										    VK_LBUTTON,
											VK_RBUTTON,
											VK_NUMPAD1,
											VK_NUMPAD2,
											VK_NUMPAD3,
											VK_NUMPAD4,
											VK_NUMPAD5,
											VK_NUMPAD6,
											VK_NUMPAD7,
											VK_NUMPAD8,
											VK_NUMPAD9
										};

int main()
{
	const int DROP_X = 1550;
	POINT downPos;
	bool letterDown[] = { false, false, false, false },
		consoleSelectionMade = false;

	// Load config and custom actions
	load();

	// Initialize console window, terminate if another instance is running
	if (bAbort = initWindow())
		return EXIT_SUCCESS;

	// Program started indicator
	if (soundEnabled)
	{
		std::thread initSound(playInitSound);
		initSound.detach();
	}

	// Print init animation
	printer.init();

	// Build key-down map
	std::unordered_map<unsigned int, bool> keyDown;

	for (int i = 0; i < virtualKeys.size(); ++i)
		keyDown[virtualKeys[i]] = false;

	// Give custom actions sound object pointer
	customActions.giveSound(&sound);

	/*
		Poll key combos
	*/

	while (true)
	{
		Sleep(10);

		// Complementary keys
		if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) && !keyDown[VK_LCONTROL])
			keyDown[VK_LCONTROL] = true;

		// Control + backspace (abort)
		if (keyDown[VK_LCONTROL] && GetAsyncKeyState(VK_BACK))
			break;	

		if (focused(GAME))
		{
			// Control click (drop)
			if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && !keyDown[VK_LBUTTON])
			{
				if (keyDown[VK_LCONTROL])
				{
					GetCursorPos(&downPos);
					Sleep(75);
					click(DROP_X, downPos.y);
					SetCursorPos(downPos.x, downPos.y);
					keyDown[VK_LBUTTON] = true;
				}
			}

			// Numpad 1 (craft rockets)
			if ((GetAsyncKeyState(VK_NUMPAD1) & 0x8000) && !keyDown[VK_NUMPAD1])
			{
				keyDown[VK_NUMPAD1] = true;
				craftRockets();
			}

			// Numpad 2 (craft blocks)
			if ((GetAsyncKeyState(VK_NUMPAD2) & 0x8000) && !keyDown[VK_NUMPAD2])
			{
				keyDown[VK_NUMPAD2] = true;
				craftBlocks();
			}

			// Numpad 3 (craft concrete)
			if ((GetAsyncKeyState(VK_NUMPAD3) & 0x8000) && !keyDown[VK_NUMPAD3])
			{
				keyDown[VK_NUMPAD3] = true;
				craftConcrete();
			}

			// Custom action keys
			for (int i = 0; i < customActions.getNumActions(); ++i)
			{
				unsigned int vkey = VK_NUMPAD4 + i;

				if ((GetAsyncKeyState(vkey) & 0x8000) && !keyDown[vkey])
				{
					customActions.execute(i);
					keyDown[vkey] = true;
				}
			}
		}
		// Console selections
		if (focused(CONSOLE))
		{
			// Add new custom action
			if ((GetAsyncKeyState('A') & 0x8000) && !letterDown[0])
			{
				letterDown[0] = true;

				if (customActions.getNumActions() < 6)
				{
					std::stringstream ss("");
					ss << printer.MARGIN_CONTENT << "Prepare to perform action in game.\n\n" << printer.MARGIN_CONTENT << "[n]ext, [c]ancel";
					amendMainText(ss.str());

					// Block until 'n' or 'c' pressed
					char select = '\0';

					while (true)
					{
						select = _getch();

						if (select == 'n' || select == 'c')
							break;
					}

					// Begin new custom action creation
					if (select == 'n')
					{
						customActions.recordAction();
						save();
					}

					// Resize window based on number of custom actions
					resizeWindow(windowWidth, BASE_WINDOW_HEIGHT + (WINDOW_HEIGHT_INCREMENT * customActions.getNumActions()));

					consoleSelectionMade = true;
				}

				else
				{
					up(2);
					std::cout << printer.backString << printer.MARGIN_CONTENT << ("(max custom actions reached)");
					Sleep(PAUSE_DURATION);
				}

				system("cls");
				printer.printText();
			}

			// Delete custom action
			if ((GetAsyncKeyState('D') & 0x8000) && !letterDown[1])
			{
				letterDown[1] = true;

				up(2);
				std::cout << printer.backString << printer.MARGIN_CONTENT;

				if (customActions.getNumActions())
				{

					int selectedIndex = -1;
					char select = '\0';

					if (customActions.getNumActions() > 1)
					{
						std::cout << "Delete which? (press 4-" << customActions.getNumActions()+3 << ")";

						select = '\0';

						do
						{
							select = _getch();

						} while (select < '4' || select > '4' + (customActions.getNumActions() - 1));

						selectedIndex = (int)select - 52;	
					}

					else
						selectedIndex = 0;

					if (selectedIndex > customActions.getNumActions())
						continue;

					std::cout << printer.backString << printer.MARGIN_CONTENT << "Delete (numpad" << selectedIndex + 4 << ") : " << customActions.getActionName(selectedIndex) << "? (y/n)";

					select = '\0';

					do
					{
						select = _getch();

					} while (select != 'y' && select != 'n');

					if (select == 'y')
					{
						if (selectedIndex < customActions.getNumActions())
						{
							customActions.remove(selectedIndex);
							save();
						}
					}
				}

				else
				{
					std::cout << ("(no custom actions exist)");
					Sleep(PAUSE_DURATION);
				}

				system("cls");
				printer.printText();
			}

			// Modify custom action
			if ((GetAsyncKeyState('M') & 0x8000) && !letterDown[2])
			{
				letterDown[2] = true;

				

				up(2);
				std::cout << printer.backString << printer.MARGIN_CONTENT;

				if (customActions.getNumActions() > 0)
				{
					int selectedIndex = -1;
					char select = '\0';

					if (customActions.getNumActions() > 1)
					{
						std::cout << "Modify which? (press 4-" << customActions.getNumActions()+3 << ")";

						select = '\0';

						do
						{
							select = _getch();

						} while (select < '4' || select > '4' + (customActions.getNumActions() - 1));

						selectedIndex = (int)select - 52;

						
					}

					else
						selectedIndex = 0;

					if (selectedIndex > customActions.getNumActions())
						continue;

					std::cout << printer.backString << printer.MARGIN_CONTENT << "Modify " << customActions.getActionName(selectedIndex) << "? (y/n)";

					select = '\0';

					do
					{
						select = _getch();

					} while (select != 'y' && select != 'n');

					

					if (select == 'y')
					{
						customActions.modify(selectedIndex);
						save();
					}
				}

				else
				{
					std::cout << ("(no custom actions exist)");
					Sleep(PAUSE_DURATION);
				}

				system("cls");
				printer.printText();
			}

			// Settings
			if ((GetAsyncKeyState('S') & 0x8000) && !letterDown[3])
			{
				letterDown[3] = true;

				bool back = false;
				char selection = '\0';

				

				while (true)
				{
					system("cls");
					printer.printText(printer.BANNER);
					printer.printText(printer.BOUNDARY_TOP);
					std::cout << "\n\n\n"

						<< printer.MARGIN_CONTENT << "[1] : Sound            " << (soundEnabled ? "ENABLED" : "DISABLED") << "\n"
						<< printer.MARGIN_CONTENT << "[2] : Window border    " << (borderEnabled ? "ENABLED" : "DISABLED") << "\n"
						<< printer.MARGIN_CONTENT << "[3] : Start hidden     " << (startHidden ? "ENABLED" : "DISABLED") << "\n\n\n"

						<< printer.MARGIN_CONTENT << "[esc] : Back\n\n\n";

					std::cout << printer.MARGIN_CONTENT << printer.MARGIN_CONTENT << printer.MARGIN_CONTENT << "     Written by entheo6\n";

					printer.printText(printer.BOUNDARY_BOTTOM);

					// Input validation
					do
					{
						selection = _getch();

					} while (selection != '1' && selection != '2' && selection != '3' && selection != char(27));

					// Sound (1)
					if (selection == '1')
					{
						config[0] = soundEnabled = !soundEnabled;
					}

					// Window border
					else if (selection == '2')
					{
						toggleBorder(config[1] = borderEnabled = !borderEnabled);
					}

					// Start hidden
					else if (selection == '3')
					{
						config[2] = startHidden = !startHidden;
					}

					// Back (esc)
					if (selection == char(27))
						back = true;

					if (back)
						break;
				}

				save();

				system("cls");
				printer.printText();
			}
		}

		// Update key-down map
		for (int i = 0; i < virtualKeys.size(); ++i)
		{
			if (!(GetAsyncKeyState(virtualKeys[i]) & 0x8000) && keyDown[virtualKeys[i]])
				keyDown[virtualKeys[i]] = false;			
		}

		// Extra key-down flags for letters
		if (!(GetAsyncKeyState('A') & 0x8000) && letterDown[0])
			letterDown[0] = false;

		if (!(GetAsyncKeyState('D') & 0x8000) && letterDown[1])
			letterDown[1] = false;

		if (!(GetAsyncKeyState('M') & 0x8000) && letterDown[2])
			letterDown[2] = false;

		if (!(GetAsyncKeyState('S') & 0x8000) && letterDown[3])
			letterDown[3] = false;

		// Reset main text after returning from different console state
		if (consoleSelectionMade)
		{
			system("cls");
			printer.printText();
			consoleSelectionMade = false;
		}
	}

	// Program terminated indicator
	if (soundEnabled)
	{
		ShowWindow(GetConsoleWindow(), SW_HIDE);
		sound.playSound(sound.CLOSE);
	}

	// Debug
	//while (true)
	//{
	//	GetCursorPos(&downPos);
	//	system("cls");
	//	std::cout << "x: " << downPos.x << ", y: " << downPos.y;
	//	Sleep(50);
	//}

	ReleaseMutex(mutex);
	CloseHandle(mutex);

	return EXIT_SUCCESS;
}

void sigint(int s)
{
	signal(s, SIG_IGN);
}

void removeScrollbar()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	COORD bufferSize = { csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1 };
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
}

void toggleBorder(bool enabled)
{
	HWND window = GetConsoleWindow();
	LONG style = GetWindowLongPtr(window, GWL_STYLE);

	if (!enabled)
	{	
		style &= ~(WS_BORDER | WS_SIZEBOX | WS_DLGFRAME);
		SetWindowLongPtr(window, GWL_STYLE, style);
		SetWindowPos(window, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

		windowHeight = BASE_WINDOW_HEIGHT;
		resizeWindow();

		removeScrollbar();
	}

	else
	{
		style |= WS_OVERLAPPEDWINDOW;
		SetWindowLongPtr(window, GWL_STYLE, style);
		SetWindowPos(window, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

		windowHeight = BASE_WINDOW_HEIGHT + WINDOW_HEIGHT_ADJUST;
		resizeWindow();
	}
}

void hideCursor()
{
	// Hide cursor
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(out, &cursorInfo);
}

void load()
{
	persistence.load(config, customActions.getActions());

	soundEnabled = config[0];
	borderEnabled = config[1];
	startHidden = config[2];
}

void save()
{
	config[0] = soundEnabled;
	config[1] = borderEnabled;
	config[2] = startHidden;

	persistence.save(config, customActions.getActions());
}

bool initWindow()
{
	// Multiple instance check
	mutex = CreateMutexA(NULL, true, instanceId);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(mutex);
		return true;
	}

	// Capture SIGINT to prevent termination from ctrl-c
	signal(SIGINT, sigint);

	// Disable quick edit mode
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD prev_mode;
	GetConsoleMode(hInput, &prev_mode);
	SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));

	// Allow extended ascii, resize window, set title and color
	SetConsoleOutputCP(437);
	system("title drop & color 04");

	if (!borderEnabled)
	{
		// Remove window border
		toggleBorder(false);

		// Remove scroll bar
		removeScrollbar();
	}

	// Set transparency
	HWND window = GetConsoleWindow();
	SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(window, 0, WINDOW_ALPHA, LWA_ALPHA);

	// Print main text
	//printer.printText();
	resizeWindow();

	// Spawn visibility thread
	windowVisible = !startHidden;
	ShowWindow(window, (windowVisible ? SW_SHOW : SW_HIDE));
	std::thread visibilityThread(pollVisibility);
	visibilityThread.detach();

	return false;
}

void playInitSound()
{
	PlaySound(MAKEINTRESOURCE((startHidden ? IDR_WAVE2 : IDR_WAVE1)), GetModuleHandle(NULL), SND_RESOURCE | SND_SYNC);
}

void resizeWindow()
{
	HWND window = GetConsoleWindow();

	// Resize and center window
	MoveWindow(window, (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2, (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2, windowWidth, windowHeight, TRUE);

	scrollUp();
	hideCursor();
}

void resizeWindow(int width, int height)
{
	windowWidth = width;
	windowHeight = height;

	resizeWindow();
}

void scrollUp()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	SMALL_RECT sr;
	sr.Left = 0;
	sr.Top = 0;
	sr.Right = csbi.dwSize.X - 1;
	sr.Bottom = csbi.srWindow.Bottom - csbi.srWindow.Top;

	SetConsoleWindowInfo(hConsole, TRUE, &sr);
}

void amendMainText(std::string text)
{
	std::string back = "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",
		space = "                                                                                         ";
	
	std::cout << "\x1b[A" << back << space << "              " << back
		      << "\x1b[A" << back << space << "              " << back
		      << text << "\n" << "          " << static_cast<char>(192) << space << static_cast<char>(217);

}

void pollVisibility()
{
	bool keyDown = false;

	while (!bAbort)
	{
		if ((GetAsyncKeyState(VK_F9) & 0x8000) && !keyDown)
		{
			windowVisible = !windowVisible;
			ShowWindow(GetConsoleWindow(), (windowVisible ? SW_SHOW : SW_HIDE));
			if (windowVisible)
				SetForegroundWindow(GetConsoleWindow());
			keyDown = true;
		}

		else if (!(GetAsyncKeyState(VK_F9) & 0x8000) && keyDown)
			keyDown = false;

		Sleep(THROTTLE_DEFAULT);
	}
}

bool focused(FOCUS_TYPE type)
{
	const int MAX_LENGTH = 256;
	char title[MAX_LENGTH];

	HWND hwnd = GetForegroundWindow();

	std::string searchTerm = (type == GAME ? "MINECRAFT" : "DROP");

	int length = GetWindowTextA(hwnd, title, MAX_LENGTH);

	if (length)
	{
		std::string str(title);

		for (int i = 0; i < str.length(); ++i)
			str[i] = std::toupper(str[i]);

		if (str.find(searchTerm) != std::string::npos)
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