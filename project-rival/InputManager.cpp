#include "InputManager.h"

/**
 * @file InputManager.cpp
 * @brief Implements the global input facade used by gameplay code.
 */

Gamepad InputManager::s_pad0{ 0 };

/**
 * @brief Polls input devices and updates per-frame button/axis state.
 */
void InputManager::update()
{
	s_pad0.update();
}

/**
 * @brief Returns the primary gamepad instance (index 0).
 * @return Reference to the internally stored `Gamepad`.
 */
Gamepad& InputManager::pad()
{
	return s_pad0;
}
