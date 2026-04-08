#include "InputManager.h"

Gamepad InputManager::s_pad0{ 0 };

void InputManager::update()
{
	s_pad0.update();
}

Gamepad& InputManager::pad()
{
	return s_pad0;
}
