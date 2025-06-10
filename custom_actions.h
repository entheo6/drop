#pragma once

#include <vector>
#include <iostream>
#include <Windows.h>

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

	struct vec2
	{
		int x, y;

		vec2()
			:x(-1), y(-1) {}
		vec2(int a, int b) { x = a; y = b; }
	};

	struct Action
	{
		struct ActionStep
		{
			ACTION_TYPE type;
			vec2 coords;

			ActionStep()
				:type(UNINITIALIZED){}
			ActionStep(ACTION_TYPE t, vec2 c)
				:type(t), coords(c) {}
		};

		std::vector<ActionStep> sequence;

		Action() {}
		void pushStep(ACTION_TYPE type, vec2 coords)
		{
			ActionStep step(type, coords);
			sequence.push_back(step);
		}
	};

// Implementation
private:
	std::vector<Action> actions;

// Interface
public:
	CustomActions() {}

	void RecordAction()
	{
		system("cls");

		std::cout << "\n\n  RECORDING MOUSE EVENTS\n\n"

		  			  << "  [esc] : abort\n"
					  << "  [enter] : finish\n\n"

					  << "0 events recorded";
	}
};
