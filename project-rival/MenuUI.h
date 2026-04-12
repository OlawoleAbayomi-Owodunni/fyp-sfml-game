#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum MenuScreen
{
	MAIN_MENU_SCREEN,
	GAMEPLAY_SCREEN,
	PAUSE_MENU_SCREEN,
	GAME_OVER_SCREEN,

	SCREEN_COUNT
};

enum MenuAction
{
	ACTION_NONE,
	ACTION_NEW_GAME,
	ACTION_CONTINUE,
	ACTION_EXIT,
	ACTION_RESUME,
	ACTION_HUB_WORLD,
	ACTION_MAIN_MENU
};

struct MenuButton
{
	sf::RectangleShape shape;
	sf::Text label;
	MenuAction action;
};

class MenuUI
{
public:
	void init(const sf::Font& font, const sf::View& defaultView);

	void setScreen(MenuScreen screen);

	MenuScreen screen() const { return m_screen; }
	bool isGameplayScreen() const { return m_screen == MenuScreen::GAMEPLAY_SCREEN; }

	void processEvent(const sf::Event& event, sf::RenderWindow& window);
	void processControllerNavigation(bool upPressed, bool downPressed, bool activatePressed);

	MenuAction consumeAction();

	void render(sf::RenderWindow& window);

private:
	void layoutMenu(std::vector<MenuButton>& buttons, const sf::View& defaultView, float startY);

	std::vector<MenuButton>* getActiveButtons();

	int* getActiveSelection();

	void moveSelection(int direction);

	void updateHover(const sf::Vector2f& worldPos);

	void activateSelectedButton();

	void updateMenuCursorPosition();

	const sf::Font* m_font{ nullptr };
	MenuScreen m_screen{ MenuScreen::MAIN_MENU_SCREEN };
	MenuAction m_pendingAction{ MenuAction::ACTION_NONE };

	std::vector<MenuButton> m_mainMenuButtons;
	std::vector<MenuButton> m_pauseMenuButtons;
	std::vector<MenuButton> m_gameOverButtons;

	int m_mainMenuSelection{ 0 };
	int m_pauseMenuSelection{ 0 };
	int m_gameOverSelection{ 0 };

	sf::RectangleShape m_menuCursor;
};
