#include "custom_actions.h"

CustomActions::CustomActions()
{
	printer = Printer(this);
}

void CustomActions::recordAction()
{
	bool shift = false,
		left = false,
		right = false,
		returnBreak = false;
	int count = 0;
	char selection;
	POINT cursorPos;
	Action actionBuffer;
	std::string id = "";

	system("cls");

	printer.printText(printer.BANNER);
	printer.printText(printer.BOUNDARY_TOP);

	std::cout << "\n\n\n\n"
		<< printer.MARGIN_CONTENT << "Press [F10] in game to begin recording.\n\n"

		<< printer.MARGIN_CONTENT << "Press [F10] again when finished.\n\n"

		<< printer.MARGIN_CONTENT << "Press [esc] to go back.\n\n\n\n";

	printer.printText(printer.BOUNDARY_BOTTOM);


	// Block until esc or f10 pressed
	while (true)
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			return;

		if (GetAsyncKeyState(VK_F10) & 0x8000)
			break;
	}

	Beep(700, 100);
	Beep(750, 100);
	system("cls");

	printer.printText(printer.BANNER);
	printer.printText(printer.BOUNDARY_TOP);
	std::cout << "\n\n\n\n"
		<< printer.MARGIN_CONTENT << "RECORDING MOUSE EVENTS\n\n"

		<< printer.MARGIN_CONTENT << "[esc] : Abort\n"
		<< printer.MARGIN_CONTENT << "[F10] : Finish\n\n"

		<< printer.MARGIN_CONTENT << "0 events recorded\n\n\n\n";

	printer.printText(printer.BOUNDARY_BOTTOM);

	up(5);

	while (true)
	{
		Sleep(THROTTLE_DEFAULT);

		shift = left = right = false;

		if (GetAsyncKeyState(VK_LBUTTON))
		{
			left = true;

			if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
				shift = true;
		}

		if (GetAsyncKeyState(VK_RBUTTON))
			right = true;

		if (left || right)
		{
			GetCursorPos(&cursorPos);
			actionBuffer.pushStep((shift ? SHIFT_CLICK : (left ? LEFT_CLICK : RIGHT_CLICK)), vec2(cursorPos.x, cursorPos.y));
			std::cout << BACK_STRING << "\b\b\b\b\b\b\b\b" << printer.MARGIN_CONTENT << ++count << " event" << (count+1 != 1 ? "s" : "") << " recorded";
			//PlaySound(MAKEINTRESOURCE(IDR_WAVE2), GetModuleHandle(NULL), SND_RESOURCE | SND_SYNC);
		}

		if (GetAsyncKeyState(VK_F10))
			break;

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			Beep(750, 100);
			Beep(600, 100);
			std::cout << BACK_STRING << "\b\b\b\b\b\b\b\b" << printer.MARGIN_CONTENT << "Aborting...\n\n";
			Sleep(PAUSE_DURATION);
			return;
		}
	}

	Beep(750, 100);
	Beep(700, 100);

	SetForegroundWindow(GetConsoleWindow());

	while (id == "")
	{
		system("cls");
		printer.printText(printer.BANNER);
		printer.printText(printer.BOUNDARY_TOP);
		std::cout << "\n\n\n\n" << printer.MARGIN_CONTENT << "RECORDING COMPLETE\n\n\n\n\n\n\n";
		printer.printText(printer.BOUNDARY_BOTTOM);

		up(6);
		std::cout << printer.MARGIN_CONTENT << "Enter name for new action: ";

		std::getline(std::cin, id);
	}

	actionBuffer.setId(id);

	system("cls");

	printer.printText(printer.BANNER);
	printer.printText(printer.BOUNDARY_TOP);

	do
	{
		selection = '0';

		std::cout << "\n\n\n" << printer.MARGIN_CONTENT << "New custom action: " << id << "\n\n"

			<< printer.MARGIN_CONTENT << "1. View steps\n"
			<< printer.MARGIN_CONTENT << "2. Change delay\n"
			<< printer.MARGIN_CONTENT << "3. Cancel, discard\n"
			<< printer.MARGIN_CONTENT << "4. Done, save custom action\n\n\n\n";

		printer.printText(printer.BOUNDARY_BOTTOM);

		selection = _getch();

		while (selection != '1' && selection != '2' && selection != '3' && selection != '4');

		// View steps (1)
		if (selection == '1' || selection == '2')
		{
			system("cls");

			printer.printText(printer.BANNER);
			printer.printText(printer.BOUNDARY_TOP);
			std::cout << printer.MARGIN_CONTENT << "(" << id << ") " << count << " STEPS\n\n";

			for (int i = 0; i < actionBuffer.sequence.size(); ++i)
			{
				std::cout << printer.MARGIN_CONTENT << "[" << (i + 1) << "] (" << actionBuffer.sequence[i].getTypeString()
					<< ") (" << actionBuffer.sequence[i].coords.x << ", " << actionBuffer.sequence[i].coords.y << ") Delay: " << actionBuffer.sequence[i].delay << "ms\n";
			}

			std::cout << "\n";
		}

		// Change delay (2)
		if (selection == '2')
		{
			int input = -1,
				newDelay = -1;

			std::cout << "\n\n\n\n\n";

			printer.printText(printer.BOUNDARY_BOTTOM);

			up(5);

			// Step selection input validation
			do
			{
				std::cout << "\x1b[A" << BACK_STRING << '\r' << printer.MARGIN_CONTENT << "Enter step to modify: ";

				std::cin >> input;

			} while (input < 1 || input > actionBuffer.sequence.size());

			// Delay input validation
			do
			{
				std::cout << "\x1b[A" << '\r' << BACK_STRING;
				std::cout << '\r' << printer.MARGIN_CONTENT << "Enter new delay for step " << input << " (current " << actionBuffer.sequence[input - 1].delay << " milliseconds): ";
				std::cin >> newDelay;

			} while (newDelay < 0 || newDelay > MAX_DELAY);

			actionBuffer.sequence[input - 1].delay = newDelay;
			std::cout << "\x1b[A" << BACK_STRING << '\r' << printer.MARGIN_CONTENT << "Saved new value...\n\n";
			Sleep(PAUSE_DURATION);
			system("cls");
			printer.printText(printer.BANNER);
			printer.printText(printer.BOUNDARY_TOP);
		}

	} while (selection != '3' && selection != '4');

	// Done, add new action (4)
	if (selection == '4')
	{
		pushAction(actionBuffer);
		up(3);
		std::cout << BACK_STRING << "\x1b[A" << BACK_STRING << '\n';
		std::cout << '\r' << printer.MARGIN_CONTENT << "Added new custom action '" << actionBuffer.id << "'...\n\n";
		Sleep(PAUSE_DURATION);
	}
}

void CustomActions::execute(int index)
{
	if (index >= actions.size())
		return;

	Action::ActionStep currentStep;

	for (int i = 0; i < actions[index].sequence.size(); ++i)
	{
		currentStep = actions[index].sequence[i];

		// Move cursor
		SetCursorPos(currentStep.coords.x, currentStep.coords.y);

		// Throttle
		Sleep(currentStep.delay);

		// Click
		switch (currentStep.type)
		{
		case LEFT_CLICK:
			mouse_event(MOUSEEVENTF_LEFTDOWN, currentStep.coords.x, currentStep.coords.y, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, currentStep.coords.x, currentStep.coords.y, 0, 0);
			break;

		case RIGHT_CLICK:
			mouse_event(MOUSEEVENTF_RIGHTDOWN, currentStep.coords.x, currentStep.coords.y, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTUP, currentStep.coords.x, currentStep.coords.y, 0, 0);
			break;

		case SHIFT_CLICK:
			INPUT input[4] = {};

			// Shift down
			input[0].type = INPUT_KEYBOARD;
			input[0].ki.wVk = VK_SHIFT;
			input[0].ki.dwFlags = 0;

			// Left button down
			input[1].type = INPUT_MOUSE;
			input[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

			// Left button up
			input[2].type = INPUT_MOUSE;
			input[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

			// Shift up
			input[3].type = INPUT_KEYBOARD;
			input[3].ki.wVk = VK_SHIFT;
			input[3].ki.dwFlags = KEYEVENTF_KEYUP;

			SendInput(4, input, sizeof(INPUT));
			break;
		}
	}
}

void CustomActions::modify(int index)
{
	int input = -1,
		newDelay = -1;

	system("cls");
	printer.printText(printer.BANNER);
	printer.printText(printer.BOUNDARY_TOP);

	std::cout << "\n\n" << printer.MARGIN_CONTENT << "(" << actions[index].id << ") " << actions[index].sequence.size() << " STEPS\n\n";

	for (int i = 0; i < actions[index].sequence.size(); ++i)
	{
		std::cout << printer.MARGIN_CONTENT << "[" << (i + 1) << "] (" << actions[index].sequence[i].getTypeString()
			<< ") (" << actions[index].sequence[i].coords.x << ", " << actions[index].sequence[i].coords.y << ") Delay: " << actions[index].sequence[i].delay << "ms\n";
	}

	std::cout << "\n\n\n\n";
	printer.printText(printer.BOUNDARY_BOTTOM);
	up(3);
	
	// Step selection input validation
	do
	{
		std::cout << "\x1b[A" << BACK_STRING << '\r' << printer.MARGIN_CONTENT << "Enter step to modify: ";
		std::cin >> input;

	} while (input < 1 || input > actions[index].sequence.size());

	// Delay input validation
	do
	{
		std::cout << "\x1b[A" << '\r' << BACK_STRING;
		std::cout << '\r' << printer.MARGIN_CONTENT << "Enter new delay for step " << input << " (current " << actions[index].sequence[input - 1].delay << " milliseconds): ";
		std::cin >> newDelay;

	} while (newDelay < 0 || newDelay > MAX_DELAY);

	actions[index].sequence[input - 1].delay = newDelay;
	std::cout << "\x1b[A" << BACK_STRING << '\r' << printer.MARGIN_CONTENT << "Saved new value...\n\n";
	Sleep(PAUSE_DURATION);
	system("cls");
	printer.printText();
}