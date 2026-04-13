#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "Quest.h"

/**
 * @file MenuUI.h
 * @brief Declares menu UI types for screens, actions, and button data.
 */

/**
 * @brief High-level menu screens.
 */
enum MenuScreen
{
	MAIN_MENU_SCREEN,
	GAMEPLAY_SCREEN,
	PAUSE_MENU_SCREEN,
	GAME_OVER_SCREEN,
	QUEST_BOARD_SCREEN,

	SCREEN_COUNT
};

/**
 * @brief Actions produced by the menu UI.
 */
enum MenuAction
{
	ACTION_NONE,
	ACTION_NEW_GAME,
	ACTION_CONTINUE,
	ACTION_EXIT,
	ACTION_RESUME,
	ACTION_HUB_WORLD,
	ACTION_MAIN_MENU,
	ACTION_QUEST_0,
	ACTION_QUEST_1,
	ACTION_QUEST_2
};

/**
 * @brief Renderable button representation and its associated action.
 */
struct MenuButton
{
	sf::RectangleShape shape;
	sf::Text label;
	MenuAction action;
};

class MenuUI
{
public:
	MenuUI(const sf::Font& font, const sf::View& defaultView);
	void init(const sf::Font& font, const sf::View& defaultView);

	void setScreen(MenuScreen screen);

	MenuScreen screen() const { return m_screen; }
	bool isGameplayScreen() const { return m_screen == MenuScreen::GAMEPLAY_SCREEN; }

	void setQuestBoardQuests(const std::vector<QuestData>& quests);

	void processEvent(const sf::Event& event, sf::RenderWindow& window);
	void processControllerNavigation(bool upPressed, bool downPressed, bool activatePressed, bool backPressed);

	MenuAction consumeAction();

	void render(sf::RenderWindow& window);

private:
	void layoutMenu(std::vector<MenuButton>& buttons, const sf::View& defaultView, float startY);
	void layoutQuestBoard(const sf::View& defaultView);
	void updateQuestBoardButtons();
	void renderQuestBoard(sf::RenderWindow& window);

	std::vector<MenuButton>* getActiveButtons();

	int* getActiveSelection();

	void moveSelection(int direction);

	void updateHover(const sf::Vector2f& worldPos);

	void activateSelectedButton();

	void updateMenuCursorPosition();

	sf::Font m_font;
	sf::View m_defaultView;
	MenuScreen m_screen{ MenuScreen::MAIN_MENU_SCREEN };
	MenuAction m_pendingAction{ MenuAction::ACTION_NONE };

	std::vector<MenuButton> m_mainMenuButtons;
	std::vector<MenuButton> m_pauseMenuButtons;
	std::vector<MenuButton> m_gameOverButtons;
	std::vector<MenuButton> m_questBoardButtons;

	int m_mainMenuSelection{ 0 };
	int m_pauseMenuSelection{ 0 };
	int m_gameOverSelection{ 0 };
	int m_questBoardSelection{ 0 };

	std::vector<QuestData> m_questBoardQuests{};
	sf::RectangleShape m_questBoardPanel;
	sf::RectangleShape m_questBoardDivider;
	sf::Text m_questBoardTitle{ m_font };
	sf::Text m_questBoardLore{ m_font };
	sf::Text m_questBoardGameplay{ m_font };
	sf::Text m_questBoardReward{ m_font };
	#pragma region Quest System
	void applyQuestBoardSelection(int index);
	#pragma endregion

	sf::RectangleShape m_menuCursor;
};
