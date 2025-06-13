#include "persistence.h"

Persistence::Persistence()
{
	try
	{
		std::string userProfile = std::getenv("LOCALAPPDATA");
		if (!std::filesystem::exists(userProfile + "\\drop"))
			std::filesystem::create_directories(userProfile + "\\drop");
		savePath = userProfile + "\\drop\\persistence";
	}

	catch (std::exception& e)
	{
		MessageBoxA(GetConsoleWindow(), e.what(), "Error Creating Save File", MB_OK);
	}

	catch (...)
	{
		std::string msg("Failed to create save file in ");
		msg += savePath;
		MessageBoxA(GetConsoleWindow(), msg.c_str(), "Error Saving", MB_OK);
	}
}

void Persistence::save(bool config[], std::vector<CustomActions::Action> actions)
{
	try
	{
		std::string idBuffer;
		std::ofstream outFile(savePath.c_str(), std::ios::out);

		// Save config
		outFile << (int)config[0] << " " << (int)config[1] << " " << (int)config[2] << '\n';

		// Save custom actions
		for (int i = 0; i < actions.size(); ++i)
		{
			idBuffer = actions[i].id;
			std::replace(idBuffer.begin(), idBuffer.end(), ' ', '@');

			outFile << "#" << idBuffer << " " << actions[i].sequence.size() << " ";

			for (int j = 0; j < actions[i].sequence.size(); ++j)
			{
				outFile << (int)actions[i].sequence[j].type << " " << actions[i].sequence[j].coords.x << " " << actions[i].sequence[j].coords.y << " " << actions[i].sequence[j].delay << " ";
			}

			outFile << '\n';
		}
	}

	catch (std::exception& e)
	{
		MessageBoxA(GetConsoleWindow(), e.what(), "Error Saving", MB_OK);
	}

	catch (...)
	{
		MessageBoxA(GetConsoleWindow(), "Unknown exception thrown while attempting to save data.", "Error Saving", MB_OK);
	}
}

void Persistence::load(bool(&config)[3], std::vector<CustomActions::Action>& actions)
{
	try
	{
		if (!std::filesystem::exists(savePath.c_str()))
			return;

		std::string lineBuffer, idBuffer;
		int type, x, y, delay;

		std::ifstream inFile(savePath.c_str(), std::ios::in);

		// Load config
		inFile >> config[0] >> config[1] >> config[2];

		// Load custom actions
		getline(inFile, lineBuffer, '#');

		while (!inFile.eof())
		{
			CustomActions::Action currentAction;
			int numSteps;

			inFile >> idBuffer >> numSteps;

			std::replace(idBuffer.begin(), idBuffer.end(), '@', ' ');

			currentAction.id = idBuffer;

			for (int i = 0; i < numSteps; ++i)
			{
				inFile >> type >> x >> y >> delay;
				currentAction.pushStep((CustomActions::ACTION_TYPE)type, vec2(x, y));
				currentAction.sequence[currentAction.sequence.size() - 1].delay = delay;
			}

			getline(inFile, lineBuffer, '#');

			actions.push_back(currentAction);
		}
	}

	catch (std::exception& e)
	{
		MessageBoxA(GetConsoleWindow(), e.what(), "Error Loading", MB_OK);
	}

	catch(...)
	{
		MessageBoxA(GetConsoleWindow(), "Unknown exception thrown while attempting to load data.", "Error Loading", MB_OK);
	}
}