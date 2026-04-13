#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"glu32.lib") 


#include "Game.h"

/**
 * @file main.cpp
 * @brief Program entry point.
 */

/**
 * @brief Application entry point.
 *
 * Creates the main `Game` instance and runs the game loop.
 *
 * @param argv Command-line arguments.
 * @return Process exit code.
 */
int main(int, char* argv[])
{
	Game game;
	game.run();
}

// Comment