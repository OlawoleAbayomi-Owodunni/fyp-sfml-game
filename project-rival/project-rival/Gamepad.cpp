#include "Gamepad.h"
#include <Windows.h>
#include <Xinput.h>
#include <algorithm>
#include <cmath>

Gamepad::Gamepad(unsigned int index)
	: m_index(index)
{
}

void Gamepad::setDeadzone(float leftStick, float rightStick)
{
	m_leftDeadzone = std::clamp(leftStick, 0.f, 0.95f);
	m_rightDeadzone = std::clamp(rightStick, 0.f, 0.95f);
}

bool Gamepad::connected() const
{
	return m_connected;
}

float Gamepad::applyDeadzone(float value, float deadzone)
{
	if (std::fabs(value) < deadzone)
		return 0.f;

	const float sign = (value < 0.f) ? -1.f : 1.f;
	const float scaled = (std::fabs(value) - deadzone) / (1.f - deadzone);
	return sign * std::clamp(scaled, 0.f, 1.f);
}

void Gamepad::update()
{
	m_buttonsPrev = m_buttonsNow;

	XINPUT_STATE state{};
	const DWORD result = XInputGetState(m_index, &state);
	m_connected = (result == ERROR_SUCCESS);

	if (!m_connected)
	{
		m_buttonsNow = 0;
		m_left = sf::Vector2f(0.f, 0.f);
		m_right = sf::Vector2f(0.f, 0.f);
		m_lt = 0.f;
		m_rt = 0.f;
		return;
	}

	m_buttonsNow = state.Gamepad.wButtons;

	// sticks [-32768..32767] -> [-1..1]
	const float lx = static_cast<float>(state.Gamepad.sThumbLX) / 32767.f;
	const float ly = static_cast<float>(state.Gamepad.sThumbLY) / 32767.f;
	const float rx = static_cast<float>(state.Gamepad.sThumbRX) / 32767.f;
	const float ry = static_cast<float>(state.Gamepad.sThumbRY) / 32767.f;

	// deadzone + SFML-friendly Y (up = -)
	m_left = sf::Vector2f(applyDeadzone(lx, m_leftDeadzone), -applyDeadzone(ly, m_leftDeadzone));
	m_right = sf::Vector2f(applyDeadzone(rx, m_rightDeadzone), -applyDeadzone(ry, m_rightDeadzone));

	// triggers [0..255] -> [0..1]
	m_lt = static_cast<float>(state.Gamepad.bLeftTrigger) / 255.f;
	m_rt = static_cast<float>(state.Gamepad.bRightTrigger) / 255.f;
}

sf::Vector2f Gamepad::leftStick() const
{
	return m_left;
}

sf::Vector2f Gamepad::rightStick() const
{
	return m_right;
}

float Gamepad::leftTrigger() const
{
	return m_lt;
}

float Gamepad::rightTrigger() const
{
	return m_rt;
}

uint16_t Gamepad::toXInputMask(GamepadButton button)
{
	switch (button)
	{
	case GamepadButton::A: return XINPUT_GAMEPAD_A;
	case GamepadButton::B: return XINPUT_GAMEPAD_B;
	case GamepadButton::X: return XINPUT_GAMEPAD_X;
	case GamepadButton::Y: return XINPUT_GAMEPAD_Y;
	case GamepadButton::LeftShoulder: return XINPUT_GAMEPAD_LEFT_SHOULDER;
	case GamepadButton::RightShoulder: return XINPUT_GAMEPAD_RIGHT_SHOULDER;
	case GamepadButton::Back: return XINPUT_GAMEPAD_BACK;
	case GamepadButton::Start: return XINPUT_GAMEPAD_START;
	case GamepadButton::LeftThumb: return XINPUT_GAMEPAD_LEFT_THUMB;
	case GamepadButton::RightThumb: return XINPUT_GAMEPAD_RIGHT_THUMB;
	case GamepadButton::DPadUp: return XINPUT_GAMEPAD_DPAD_UP;
	case GamepadButton::DPadDown: return XINPUT_GAMEPAD_DPAD_DOWN;
	case GamepadButton::DPadLeft: return XINPUT_GAMEPAD_DPAD_LEFT;
	case GamepadButton::DPadRight: return XINPUT_GAMEPAD_DPAD_RIGHT;
	default: return 0;
	}
}

bool Gamepad::down(GamepadButton button) const
{
	const uint16_t mask = toXInputMask(button);
	return (m_buttonsNow & mask) != 0;
}

bool Gamepad::pressed(GamepadButton button) const
{
	const uint16_t mask = toXInputMask(button);
	return ((m_buttonsNow & mask) != 0) && ((m_buttonsPrev & mask) == 0);
}

bool Gamepad::released(GamepadButton button) const
{
	const uint16_t mask = toXInputMask(button);
	return ((m_buttonsNow & mask) == 0) && ((m_buttonsPrev & mask) != 0);
}

void Gamepad::setRumble(float lowFreq, float highFreq)
{
	if (!m_connected)
		return;

	lowFreq = std::clamp(lowFreq, 0.f, 1.f);
	highFreq = std::clamp(highFreq, 0.f, 1.f);

	XINPUT_VIBRATION vib{};
	vib.wLeftMotorSpeed = static_cast<WORD>(lowFreq * 65535.f);
	vib.wRightMotorSpeed = static_cast<WORD>(highFreq * 65535.f);

	XInputSetState(m_index, &vib);
}