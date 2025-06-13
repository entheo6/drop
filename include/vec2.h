#pragma once

struct vec2
{
	int x, y;

	vec2()
		:x(-1), y(-1) {}
	vec2(int a, int b) { x = a; y = b; }
	vec2& operator=(const vec2& arg)
	{
		if (this != &arg)
		{
			x = arg.x;
			y = arg.y;
		}

		return *this;
	}
};