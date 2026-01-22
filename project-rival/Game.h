#pragma once

// If VS Debug build is enabled, then any block of code enclosed within 
//  the preprocessor directive #ifdef TEST_FPS is compiled and executed.
#ifdef _DEBUG
#define TEST_FPS
#endif // _DEBUG

#include <SFML/Graphics.hpp>
#include <string>

#include "player.h"
#include "Enemy.h"

using namespace std;
using namespace sf;

/// <summary>
/// @author OA-O
/// @date November 2025
/// @version 1.0
/// 
/// </summary>

struct ScreenSize
{
public:
	static const int s_width{ 1440 };

	static const int s_height{ 900 };
};

class Game
{
public:
	Game();
	void run();

protected:
	void init();
	void update(double dt);

	void render();

	void processEvents();
	void processGameEvents(const sf::Event&);

	sf::Font m_arialFont{ "ASSETS/FONTS/ariblk.ttf" };
	sf::RenderWindow m_window;

	player m_player;
	vector<Enemy> m_enemies;

#ifdef TEST_FPS
	sf::Text x_updateFPS{ m_arialFont };	// text used to display updates per second.
	sf::Text x_drawFPS{  m_arialFont };	// text used to display draw calls per second.
	sf::Time x_secondTime{ sf::Time::Zero };			// counter used to establish when a second has passed.
	int x_updateFrameCount{ 0 };						// updates per second counter.
	int x_drawFrameCount{ 0 };							// draws per second counter.
#endif // TEST_FPS


private:
	void gameStart();
};
