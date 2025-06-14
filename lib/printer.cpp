#include "printer.h"
#include "custom_actions.h"

Printer::Printer(CustomActions* ptr)
{
	customActions = ptr;

	// Build back string (for overwriting inline console contents)
	backString = "\r";
	for (int i = 0; i < 110; ++i)
		backString += ' ';
	backString += '\r';
}

Printer& Printer::operator=(const Printer& arg)
{
	if (this != &arg)
		customActions = arg.customActions;

	return *this;
}

void Printer::printText(TEXT_TYPE type)
{

	char bar1 = static_cast<char>(176),
		bar2 = static_cast<char>(177),
		bar3 = static_cast<char>(178),
		borderTL = static_cast<char>(218),
		borderTR = static_cast<char>(191),
		borderBL = static_cast<char>(192),
		borderBR = static_cast<char>(217);


	switch (type)
	{
	case BANNER:
		std::cout << "\n\n"
			<< MARGIN_BANNER << bar1 << bar1 << bar1 << "              " << bar1 << bar1 << bar1 << '\n'
			<< MARGIN_BANNER << bar1 << "           " << bar3 << "      " << bar1 << '\n'
			<< MARGIN_BANNER << bar1 << "           " << bar3 << "      " << bar1 << '\n'
			<< MARGIN_BANNER << bar1 << "           " << bar3 << "      " << bar1 << '\n'
			<< MARGIN_BANNER << bar1 << "      " << bar3 << bar3 << bar3 << bar3 << bar3 << bar3 << "      " << bar1 << '\n'
			<< MARGIN_BANNER << bar1 << "      " << bar3 << "    " << bar3 << "      " << bar1 << '\n'
			<< MARGIN_BANNER << bar1 << "      " << bar3 << "    " << bar3 << "      " << bar1 << '\n'
			<< MARGIN_BANNER << bar1 << "      " << bar3 << bar3 << bar3 << bar3 << bar3 << bar3 << "      " << bar1 << '\n'
			<< MARGIN_BANNER << bar1 << bar1 << bar1 << "              " << bar1 << bar1 << bar1 << "\n\n\n";

		break;

	case BOUNDARY_TOP:
		std::cout << "          " << borderTL << "                                                                                         " << borderTR << "\n";
		break;

	case MAIN_TEXT:
		std::cout << MARGIN_CONTENT << "[F9] : Toggle visibility                               [s]ettings\n\n";

		std::cout << MARGIN_CONTENT << "[ctrl + left click] : Drop inventory item\n\n";

		std::cout << MARGIN_CONTENT << "[numpad 1] : Pattern 1 (rockets)\n";
		std::cout << MARGIN_CONTENT << "[numpad 2] : Pattern 2 (blocks)\n";
		std::cout << MARGIN_CONTENT << "[numpad 3] : Pattern 3 (concrete)\n";

		for (int i = 0; i < customActions->getNumActions(); ++i)
			std::cout << MARGIN_CONTENT << "[numpad " << char(52 + i) << "] : " << customActions->getActionName(i) << "\n";

		std::cout << "\n";

		std::cout << MARGIN_CONTENT << "[esc] : Abort current operation\n";
		std::cout << MARGIN_CONTENT << "[ctrl + backspace] : Terminate\n\n";

		std::cout << MARGIN_CONTENT << "[a]dd custom action, [d]elete custom action, [m]odify custom action\n";

		break;

	case BOUNDARY_BOTTOM:
		std::cout << "          " << borderBL << "                                                                                         " << borderBR << "\n";
	}

}
void Printer::printText()
{
	printText(BANNER);
	printText(BOUNDARY_TOP);
	printText(MAIN_TEXT);
	printText(BOUNDARY_BOTTOM);
}

void Printer::init()
{
	char bar1 = static_cast<char>(176),
		bar2 = static_cast<char>(177),
		bar3 = static_cast<char>(178),
		borderTL = static_cast<char>(218),
		borderTR = static_cast<char>(191),
		borderBL = static_cast<char>(192),
		borderBR = static_cast<char>(217);

	Sleep(75);

	std::cout << "\n\n\n\n\n" << MARGIN_CONTENT << MARGIN_CONTENT << "              []";

	Sleep(PAUSE_DEFAULT);
	std::cout << "\b\b\r\x1b[A\x1b[A\x1b[A\x1b[A\x1b[A";

	std::cout << "\n\n"
		<< MARGIN_BANNER << "     " << bar2 << bar2 << bar2 << "    " << bar2 << bar2 << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << bar2 << bar2 << "    " << bar2 << bar2 << bar2 << "\n\n\n";

	Sleep(PAUSE_DEFAULT);

	system("cls");

	Sleep(PAUSE_DEFAULT);

	std::cout << "\n\n"
		<< MARGIN_BANNER << "     " << bar2 << bar2 << bar2 << "    " << bar2 << bar2 << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << "        " << bar2 << '\n'
		<< MARGIN_BANNER << "     " << bar2 << bar2 << bar2 << "    " << bar2 << bar2 << bar2 << "\n\n\n";

	Sleep(PAUSE_DEFAULT);

	system("cls");

	Sleep(PAUSE_DEFAULT);

	std::cout << "\n\n\n"
		<< MARGIN_BANNER << "            " << bar1 << "       \n"
		<< MARGIN_BANNER << "            " << bar1 << "       \n"
		<< MARGIN_BANNER << "            " << bar1 << "       \n"
		<< MARGIN_BANNER << "       " << bar1 << bar1 << bar1 << bar1 << bar1 << bar1 << "       \n"
		<< MARGIN_BANNER << "       " << bar1 << "    " << bar1 << "       \n"
		<< MARGIN_BANNER << "       " << bar1 << "    " << bar1 << "       \n"
		<< MARGIN_BANNER << "       " << bar1 << bar1 << bar1 << bar1 << bar1 << bar1 << "       \n\n\n\n";

	Sleep(PAUSE_DEFAULT);

	system("cls");

	std::cout << "\n\n"
		<< MARGIN_BANNER << bar1 << bar1 << bar1 << "              " << bar1 << bar1 << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "           " << bar2 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "           " << bar2 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "           " << bar2 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "      " << bar2 << bar2 << bar2 << bar2 << bar2 << bar2 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "      " << bar2 << "    " << bar2 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "      " << bar2 << "    " << bar2 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "      " << bar2 << bar2 << bar2 << bar2 << bar2 << bar2 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << bar1 << bar1 << "              " << bar1 << bar1 << bar1 << "\n\n\n";

	Sleep(PAUSE_DEFAULT);

	system("cls");

	std::cout << "\n\n"
		<< MARGIN_BANNER << bar1 << bar1 << bar1 << "              " << bar1 << bar1 << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "           " << bar1 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "           " << bar1 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "           " << bar1 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "      " << bar1 << bar1 << bar1 << bar1 << bar1 << bar1 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "      " << bar1 << "    " << bar1 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "      " << bar1 << "    " << bar1 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << "      " << bar1 << bar1 << bar1 << bar1 << bar1 << bar1 << "      " << bar1 << '\n'
		<< MARGIN_BANNER << bar1 << bar1 << bar1 << "              " << bar1 << bar1 << bar1 << "\n\n\n";

	std::cout << "          " << borderTL << "                                                                                         " << borderTR << "\n";
	std::cout << "          " << borderBL << "                                                                                         " << borderBR;

	Sleep(PAUSE_DEFAULT);

	std::cout << backString << MARGIN_CONTENT << "[F9] : Toggle visibility                               [s]ettings\n";
	std::cout << "          " << borderBL << "                                                                                         " << borderBR;

	Sleep(PAUSE_SHORT);

	std::cout << backString << '\n' << MARGIN_CONTENT << "[ctrl + left click] : Drop inventory item\n";
	std::cout << "          " << borderBL << "                                                                                         " << borderBR;

	Sleep(PAUSE_SHORT);

	std::cout << backString << '\n' << MARGIN_CONTENT << "[numpad 1] : Pattern 1 (rockets)\n";
	std::cout << "          " << borderBL << "                                                                                         " << borderBR;

	Sleep(PAUSE_SHORT);

	std::cout << backString << MARGIN_CONTENT << "[numpad 2] : Pattern 2 (blocks)\n";
	std::cout << "          " << borderBL << "                                                                                         " << borderBR;

	Sleep(PAUSE_SHORT);

	std::cout << backString << MARGIN_CONTENT << "[numpad 3] : Pattern 3 (concrete)\n";
	std::cout << "          " << borderBL << "                                                                                         " << borderBR;

	Sleep(PAUSE_SHORT);

	for (int i = 0; i < customActions->getNumActions(); ++i)
	{
		std::cout << backString << MARGIN_CONTENT << "[numpad " << char(52 + i) << "] : " << customActions->getActionName(i) << "\n";
		std::cout << "          " << borderBL << "                                                                                         " << borderBR;

		Sleep(PAUSE_SHORT);
	}

	std::cout << backString << "\n";

	std::cout << backString << backString << MARGIN_CONTENT << "[esc] : Abort current operation\n";
	std::cout << "          " << borderBL << "                                                                                         " << borderBR;

	Sleep(PAUSE_SHORT);

	std::cout << backString << MARGIN_CONTENT << "[ctrl + backspace] : Terminate\n\n";
	std::cout << "          " << borderBL << "                                                                                         " << borderBR;

	Sleep(PAUSE_SHORT);

	std::cout << backString << MARGIN_CONTENT << "[a]dd custom action, [d]elete custom action, [m]odify custom action\n";
	std::cout << "          " << borderBL << "                                                                                         " << borderBR;

	Sleep(PAUSE_SHORT);

	system("cls");
	printText();
}