#pragma once
#include "Gamepad.h"

class InputManager
{
public:
	static void update();
	static Gamepad& pad();

private:
	static Gamepad s_pad0;
};

