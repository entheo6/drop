#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <Windows.h>

#include "custom_actions.h"

class Persistence
{
private:
	std::string savePath;

public:
	Persistence();

	void save(bool[], std::vector<CustomActions::Action>);
	void load(bool(&config)[3], std::vector<CustomActions::Action>&);
};