#pragma once
#include <SFML/System/Vector2.hpp>
#include <cstdint>

enum class GamepadButton : uint16_t
{
	A,
	B,
	X,
	Y,
	LeftShoulder,
	RightShoulder,
	Back,
	Start,
	LeftThumb,
	RightThumb,
	DPadUp,
	DPadDown,
	DPadLeft,
	DPadRight
};

class Gamepad
{
public:
	explicit Gamepad(unsigned int index = 0);

	void update();
	bool connected() const;

	sf::Vector2f leftStick() const;   // normalized [-1..1], Y is SFML-friendly (up = -)
	sf::Vector2f rightStick() const;  // normalized [-1..1], Y is SFML-friendly (up = -)

	float leftTrigger() const;        // [0..1]
	float rightTrigger() const;       // [0..1]

	bool down(GamepadButton button) const;     // held
	bool pressed(GamepadButton button) const;  // edge this frame
	bool released(GamepadButton button) const; // edge this frame

	void setRumble(float lowFreq, float highFreq); // [0..1] each

	void setDeadzone(float leftStick, float rightStick);

private:
	static uint16_t toXInputMask(GamepadButton button);
	static float applyDeadzone(float value, float deadzone);

	unsigned int m_index{ 0 };
	bool m_connected{ false };

	// deadzones in normalized units
	float m_leftDeadzone{ 0.50f };
	float m_rightDeadzone{ 0.50f };

	// cached state
	uint16_t m_buttonsNow{ 0 };
	uint16_t m_buttonsPrev{ 0 };

	sf::Vector2f m_left{ 0.f, 0.f };
	sf::Vector2f m_right{ 0.f, 0.f };

	float m_lt{ 0.f };
	float m_rt{ 0.f };
};

