#pragma once

#include <iostream>

class CustomActions;

class Printer
{
private:
	const char* MARGIN_BANNER = "                                                 ";

	char bar1 = static_cast<char>(176),
		bar2 = static_cast<char>(177),
		bar3 = static_cast<char>(178),
		borderTL = static_cast<char>(218),
		borderTR = static_cast<char>(191),
		borderBL = static_cast<char>(192),
		borderBR = static_cast<char>(217);

	CustomActions* customActions;

public:
	const char* MARGIN_CONTENT = "                      ";

	std::string backString;

	enum TEXT_TYPE
	{
		BANNER,
		BOUNDARY_TOP,
		MAIN_TEXT,
		BOUNDARY_BOTTOM,
	};

	Printer() {}
	Printer(CustomActions*);
	void printText(TEXT_TYPE);
	void printText();
};
