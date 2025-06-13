#pragma once

#include <sstream>
#include <vector>
#include <iostream>
#include <conio.h>
#include <limits>
#include <Windows.h>

#include "vec2.h"
#include "printer.h"

class CustomActions
{
//	Auxiliary code
public:
	enum ACTION_TYPE
	{
		UNINITIALIZED,
		LEFT_CLICK,
		RIGHT_CLICK,
		SHIFT_CLICK
	};

	struct Action
	{
		struct ActionStep
		{
			const int DELAY_DEFAULT = 75;
			ACTION_TYPE type;
			vec2 coords;
			int delay;

			ActionStep()
				:type(UNINITIALIZED), delay(DELAY_DEFAULT){}
			ActionStep(ACTION_TYPE t, vec2 c)
				:type(t), coords(c), delay(DELAY_DEFAULT){}
			inline std::string getTypeString() { return (type == LEFT_CLICK ? "LEFT CLICK" : (type == RIGHT_CLICK ? "RIGHT CLICK" : "SHIFT CLICK")); }
			ActionStep& operator=(const ActionStep& arg)
			{
				if (this != &arg)
				{
					type = arg.type;
					coords = arg.coords;
					delay = arg.delay;
				}

				return *this;
			}
		};

		std::string id;
		std::vector<ActionStep> sequence;

		Action()
			:id("UNINITIALIZED"){}

		void pushStep(ACTION_TYPE type, vec2 coords)
		{
			ActionStep step(type, coords);
			sequence.push_back(step);
		}

		inline void setId(std::string arg) { id = arg; }
	};

// Implementation
private:
	std::vector<Action> actions;

	const int THROTTLE_DEFAULT = 100,
			  PAUSE_DURATION = 1500,
	          MAX_DELAY = 86400000,
			  MAX_ID_LENGTH = 32;

	const char* BACK_STRING = "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b                                                                                   \b\b\b	\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";

	Printer printer;

	inline void pushAction(Action arg) { actions.push_back(arg); }
	inline void up(int count) { for (int i = 0; i < count; ++i) std::cout << "\x1b[A"; }

// Interface
public:
	CustomActions();

	inline std::vector<Action>& getActions() { return actions; }
	inline int getNumActions() { return actions.size(); }
	inline const char* getActionName(int index) { return actions[index].id.c_str(); }
	inline void remove(int index) { actions.erase(actions.begin() + index); }

	void recordAction();
	void execute(int);
	void modify(int);
};
