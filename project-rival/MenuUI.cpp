#include "MenuUI.h"

#include <sstream>
#include <unordered_map>

namespace
{
	const sf::Texture* loadTexture(const std::string& texturePath)
	{
		static std::unordered_map<std::string, sf::Texture> textureCache;
		auto [it, inserted] = textureCache.try_emplace(texturePath);
		if (inserted && !it->second.loadFromFile(texturePath))
			return nullptr;
		return &it->second;
	}
}

/**
 * @file MenuUI.cpp
 * @brief Implements menu rendering, navigation, and quest board UI.
 */

/**
 * @brief Constructs the menu UI and initializes its controls and layout.
 * @param font Font used for menu labels.
 * @param defaultView Default view used as a layout reference.
 */
MenuUI::MenuUI(const sf::Font& font, const sf::View& defaultView):
	m_font(font),
	m_defaultView(defaultView)
{
	init(font, defaultView);
}

/**
 * @brief Initializes menu state, creates buttons, and lays out all screens.
 * @param font Font used for menu labels.
 * @param defaultView Default view used as a layout reference.
 */
void MenuUI::init(const sf::Font& font, const sf::View& defaultView)
{
	m_font = font;
	m_defaultView = defaultView;
	m_screen = MenuScreen::MAIN_MENU_SCREEN;
	m_mainMenuSelection = 0;
	m_pauseMenuSelection = 0;
	m_gameOverSelection = 0;
	m_questBoardSelection = 0;
	m_pendingAction = MenuAction::ACTION_NONE;

	m_menuCursor.setSize(sf::Vector2f(16.f, 16.f));
	m_menuCursor.setOrigin(m_menuCursor.getSize() / 2.f);
	m_menuCursor.setFillColor(sf::Color::Transparent);
	m_menuCursor.setOutlineThickness(3.f);
	m_menuCursor.setOutlineColor(sf::Color::Yellow);

	auto makeButton = [&](const std::string& text, MenuAction action)
		{
			MenuButton button{ sf::RectangleShape(), sf::Text(font), action };
			button.shape.setSize(sf::Vector2f(380.f, 68.f));
			button.shape.setOrigin(button.shape.getSize() / 2.f);
			button.shape.setFillColor(sf::Color::White);
			button.shape.setOutlineThickness(2.f);
			button.shape.setOutlineColor(sf::Color::White);

			button.label.setCharacterSize(28);
			button.label.setFillColor(sf::Color::White);
			button.label.setString(text);
			return button;
		};

	m_mainMenuButtons = {
		makeButton("New Game", MenuAction::ACTION_NEW_GAME),
		makeButton("Continue", MenuAction::ACTION_CONTINUE),
		makeButton("Exit", MenuAction::ACTION_EXIT)
	};

	m_pauseMenuButtons = {
		makeButton("Resume", MenuAction::ACTION_RESUME),
		makeButton("Render Mode: Textured", MenuAction::ACTION_TOGGLE_RENDER_MODE),
		makeButton("Go To Hub World", MenuAction::ACTION_HUB_WORLD),
		makeButton("Go To Main Menu", MenuAction::ACTION_MAIN_MENU)
	};

	m_gameOverButtons = {
		makeButton("Go To Hub World", MenuAction::ACTION_HUB_WORLD)
	};

	#pragma region Quest System
	m_questBoardButtons = {
		makeButton("Quest 1", MenuAction::ACTION_QUEST_0),
		makeButton("Quest 2", MenuAction::ACTION_QUEST_1),
		makeButton("Quest 3", MenuAction::ACTION_QUEST_2)
	};

	m_questBoardPanel.setFillColor(sf::Color(18, 18, 18, 240));
	m_questBoardPanel.setOutlineThickness(2.f);
	m_questBoardPanel.setOutlineColor(sf::Color::White);

	m_questBoardDivider.setFillColor(sf::Color::White);

	m_questBoardTitle.setFont(font);
	m_questBoardTitle.setCharacterSize(38);
	m_questBoardTitle.setFillColor(sf::Color::White);

	m_questBoardLore.setFont(font);
	m_questBoardLore.setCharacterSize(22);
	m_questBoardLore.setFillColor(sf::Color(220, 220, 220));

	m_questBoardGameplay.setFont(font);
	m_questBoardGameplay.setCharacterSize(24);
	m_questBoardGameplay.setFillColor(sf::Color::White);

	m_questBoardReward.setFont(font);
	m_questBoardReward.setCharacterSize(26);
	m_questBoardReward.setFillColor(sf::Color::Yellow);
	#pragma endregion

	layoutMenu(m_mainMenuButtons, defaultView, 360.f);
	layoutMenu(m_pauseMenuButtons, defaultView, 360.f);
	layoutMenu(m_gameOverButtons, defaultView, 390.f);
	layoutQuestBoard(defaultView);
	updateQuestBoardButtons();
	updateMenuCursorPosition();
}

/**
 * @brief Renders the active menu screen.
 * @param window Render target.
 */
void MenuUI::render(sf::RenderWindow& window)
{
	auto* buttons = getActiveButtons();
	int* selectedIndex = getActiveSelection();
	if (!buttons || !selectedIndex)
		return;

	const sf::View defaultView = window.getDefaultView();
	const sf::Vector2f viewCenter = defaultView.getCenter();
	const sf::Vector2f viewSize = defaultView.getSize();

	if (const sf::Texture* backdropTexture = loadTexture("ASSETS/SPRITES/UI/Menu Backdrop.png"))
	{
		sf::Sprite backdrop(*backdropTexture);
		const sf::FloatRect backdropBounds = backdrop.getGlobalBounds();
		if (backdropBounds.size.x > 0.f && backdropBounds.size.y > 0.f)
		{
			backdrop.setScale(sf::Vector2f(viewSize.x / backdropBounds.size.x, viewSize.y / backdropBounds.size.y));
			backdrop.setPosition(viewCenter - viewSize / 2.f);
			window.draw(backdrop);
		}
	}

	sf::RectangleShape overlay(viewSize);
	//set the overlay to solid black if in main menu and then to semi-transparent black if in pause or game over menu
	if (m_screen == MenuScreen::MAIN_MENU_SCREEN)
		overlay.setFillColor(sf::Color(0, 0, 0, 90));
	else
		overlay.setFillColor(sf::Color(0, 0, 0, 170));
	overlay.setOrigin(viewSize * 0.5f);
	overlay.setPosition(viewCenter);
	window.draw(overlay);

	if (m_screen == MenuScreen::QUEST_BOARD_SCREEN)
	{
		renderQuestBoard(window);
		updateMenuCursorPosition();
		window.draw(m_menuCursor);
		return;
	}

	sf::Text title(m_font);
	title.setCharacterSize(48);
	title.setFillColor(sf::Color::White);

	if (m_screen == MenuScreen::MAIN_MENU_SCREEN)
		title.setString("Space Raider");
	else if (m_screen == MenuScreen::PAUSE_MENU_SCREEN)
		title.setString("Paused");
	else
		title.setString("Game Over");

	title.setOrigin(title.getGlobalBounds().getCenter());
    title.setPosition(sf::Vector2f(viewCenter.x, viewCenter.y - 260.f));
	window.draw(title);

	if (m_screen == MenuScreen::GAME_OVER_SCREEN)
	{
		window.draw(m_gameOverKills);
		window.draw(m_gameOverTime);
		window.draw(m_gameOverCoins);
		window.draw(m_gameOverHealth);
		window.draw(m_gameOverAmmo);
		window.draw(m_gameOverHighscore);
	}

	for (int i = 0; i < static_cast<int>(buttons->size()); i++)
	{
		auto& button = (*buttons)[i];
		const bool isSelected = (i == *selectedIndex);
		button.shape.setFillColor(sf::Color::White);
		button.shape.setOutlineColor(isSelected ? sf::Color::Yellow : sf::Color::White);
		if (const sf::Texture* buttonTexture = loadTexture("ASSETS/SPRITES/UI/Button Normal.png"))
			button.shape.setTexture(buttonTexture);
		window.draw(button.shape);
		window.draw(button.label);
	}

	updateMenuCursorPosition();
	window.draw(m_menuCursor);
}

void MenuUI::setGameOverStats(int kills, float timeSeconds, int coins, int healthPacks, int ammoPacks, float highscore)
{
	const sf::Vector2f viewCenter = m_defaultView.getCenter();
	const float startY = viewCenter.y - 130.f;
	const float lineSpacing = 36.f;
	const float x = viewCenter.x;

	auto initLine = [&](sf::Text& text)
		{
			text.setFont(m_font);
			text.setCharacterSize(24);
			text.setFillColor(sf::Color::White);
			text.setOutlineThickness(1.f);
			text.setOutlineColor(sf::Color::Black);
		};

	initLine(m_gameOverKills);
	initLine(m_gameOverTime);
	initLine(m_gameOverCoins);
	initLine(m_gameOverHealth);
	initLine(m_gameOverAmmo);
	initLine(m_gameOverHighscore);

	{
		m_gameOverKills.setString("Kills: " + std::to_string(kills));
		m_gameOverKills.setOrigin(m_gameOverKills.getGlobalBounds().getCenter());
		m_gameOverKills.setPosition({ x, startY });
	}
	{
		std::ostringstream ss;
		ss.setf(std::ios::fixed);
		ss.precision(1);
		ss << "Time: " << timeSeconds << "s";
		m_gameOverTime.setString(ss.str());
		m_gameOverTime.setOrigin(m_gameOverTime.getGlobalBounds().getCenter());
		m_gameOverTime.setPosition({ x, startY + lineSpacing });
	}
	{
		m_gameOverCoins.setString("Coins collected: " + std::to_string(coins));
		m_gameOverCoins.setOrigin(m_gameOverCoins.getGlobalBounds().getCenter());
		m_gameOverCoins.setPosition({ x, startY + lineSpacing * 2 });
	}
	{
		m_gameOverHealth.setString("Health packs: " + std::to_string(healthPacks));
		m_gameOverHealth.setOrigin(m_gameOverHealth.getGlobalBounds().getCenter());
		m_gameOverHealth.setPosition({ x, startY + lineSpacing * 3 });
	}
	{
		m_gameOverAmmo.setString("Ammo packs: " + std::to_string(ammoPacks));
		m_gameOverAmmo.setOrigin(m_gameOverAmmo.getGlobalBounds().getCenter());
		m_gameOverAmmo.setPosition({ x, startY + lineSpacing * 4 });
	}
	{
		std::ostringstream ss;
		ss.setf(std::ios::fixed);
		ss.precision(1);
		ss << "Highscore: " << highscore;
		m_gameOverHighscore.setString(ss.str());
		m_gameOverHighscore.setFillColor(sf::Color::Yellow);
		m_gameOverHighscore.setOrigin(m_gameOverHighscore.getGlobalBounds().getCenter());
		m_gameOverHighscore.setPosition({ x, startY + lineSpacing * 5 });
	}
}

/**
 * @brief Changes the active screen and updates selection/layout as needed.
 * @param screen New screen.
 */
void MenuUI::setScreen(MenuScreen screen)
{
	m_screen = screen;
	if (m_screen == MenuScreen::QUEST_BOARD_SCREEN)
	{
		m_questBoardSelection = 0;
		layoutQuestBoard(m_defaultView);
		updateQuestBoardButtons();
	}
	updateMenuCursorPosition();
}

#pragma region Quest System
/**
 * @brief Sets the list of quests displayed on the quest board.
 * @param quests Quest list.
 */
void MenuUI::setQuestBoardQuests(const std::vector<QuestData>& quests)
{
	m_questBoardQuests = quests;
	updateQuestBoardButtons();
}

void MenuUI::setPauseRenderModeLabel(bool texturedMode)
{
	if (m_pauseMenuButtons.size() < 2)
		return;

	MenuButton& renderModeButton = m_pauseMenuButtons[1];
	renderModeButton.label.setString(texturedMode ? "Render Mode: Textured" : "Render Mode: Debug");

	const sf::FloatRect bounds = renderModeButton.label.getLocalBounds();
	renderModeButton.label.setOrigin({
		bounds.position.x + bounds.size.x / 2.f,
		bounds.position.y + bounds.size.y / 2.f
		});
	renderModeButton.label.setPosition(renderModeButton.shape.getPosition());
}
#pragma endregion

/**
 * @brief Processes keyboard and mouse events for menu navigation.
 * @param event SFML event.
 * @param window Window used for coordinate mapping.
 */
void MenuUI::processEvent(const sf::Event& event, sf::RenderWindow& window)
{
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		switch (keyPressed->scancode)
		{
		case sf::Keyboard::Scancode::Up:
		case sf::Keyboard::Scancode::W:
			moveSelection(-1);
			break;
		case sf::Keyboard::Scancode::Down:
		case sf::Keyboard::Scancode::S:
			moveSelection(1);
			break;
		case sf::Keyboard::Scancode::Enter:
			activateSelectedButton();
			break;
		case sf::Keyboard::Scancode::Escape:
			if (m_screen == MenuScreen::PAUSE_MENU_SCREEN || m_screen == MenuScreen::QUEST_BOARD_SCREEN)
				m_screen = MenuScreen::GAMEPLAY_SCREEN;
			break;
		default:
			break;
		}
	}

	if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
	{
		const sf::Vector2f worldPos = static_cast<sf::Vector2f>(window.mapPixelToCoords(mouseMoved->position, window.getDefaultView()));
		updateHover(worldPos);
	}

	if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
	{
		if (mousePressed->button == sf::Mouse::Button::Left)
		{
			const sf::Vector2f worldPos = static_cast<sf::Vector2f>(window.mapPixelToCoords(mousePressed->position, window.getDefaultView()));
			auto* buttons = getActiveButtons();
			int* selection = getActiveSelection();
			if (!buttons || !selection)
				return;

			for (int i = 0; i < static_cast<int>(buttons->size()); i++)
			{
				if ((*buttons)[i].shape.getGlobalBounds().contains(worldPos))
				{
					*selection = i;
					activateSelectedButton();
					break;
				}
			}
		}
	}

	updateMenuCursorPosition();
}

/**
 * @brief Processes controller navigation inputs for the active menu.
 * @param upPressed Navigate up.
 * @param downPressed Navigate down.
 * @param activatePressed Activate current selection.
 * @param backPressed Navigate back to gameplay when applicable.
 */
void MenuUI::processControllerNavigation(bool upPressed, bool downPressed, bool activatePressed, bool backPressed)
{
	if (upPressed)
		moveSelection(-1);
	if (downPressed)
		moveSelection(1);
	if (activatePressed)
		activateSelectedButton();
	if (backPressed)
		if (m_screen == MenuScreen::PAUSE_MENU_SCREEN || m_screen == MenuScreen::QUEST_BOARD_SCREEN)
			m_screen = MenuScreen::GAMEPLAY_SCREEN;
}

/**
 * @brief Returns and clears the pending menu action.
 * @return Action selected by the user since the last call.
 */
MenuAction MenuUI::consumeAction()
{
	const MenuAction action = m_pendingAction;
	m_pendingAction = MenuAction::ACTION_NONE;
	return action;
}


/**
 * @brief Lays out a vertical list of buttons centered in the view.
 * @param buttons Buttons to position.
 * @param defaultView View used for center/size reference.
 * @param startY Starting y-coordinate for the first button.
 */
void MenuUI::layoutMenu(std::vector<MenuButton>& buttons, const sf::View& defaultView, float startY)
{
	const sf::Vector2f screenCenter = defaultView.getCenter();
	const float verticalSpacing = 90.f;

	for (size_t i = 0; i < buttons.size(); i++)
	{
		auto& button = buttons[i];
		button.shape.setPosition(sf::Vector2f(screenCenter.x, startY + static_cast<float>(i) * verticalSpacing));
		button.label.setOrigin(button.label.getGlobalBounds().getCenter());
		button.label.setPosition(button.shape.getPosition());
	}
}

#pragma region Quest System
/**
 * @brief Lays out the quest board panel and its buttons/text.
 * @param defaultView View used for center/size reference.
 */
void MenuUI::layoutQuestBoard(const sf::View& defaultView)
{
	const sf::Vector2f viewCenter = defaultView.getCenter();
	const sf::Vector2f viewSize = defaultView.getSize();
	const sf::Vector2f panelSize(viewSize.x * 0.82f, viewSize.y * 0.72f);
	const sf::Vector2f panelOrigin = panelSize / 2.f;

	m_questBoardPanel.setSize(panelSize);
	m_questBoardPanel.setOrigin(panelOrigin);
	m_questBoardPanel.setPosition(viewCenter);

	const float leftWidth = panelSize.x * 0.25f;
	const float leftPadding = 22.f;
	const float buttonWidth = leftWidth - leftPadding * 2.f;
	const float buttonHeight = 72.f;
	const float buttonStartX = viewCenter.x - panelOrigin.x + leftWidth / 2.f;
	const float buttonStartY = viewCenter.y - panelOrigin.y + 170.f;
	const float buttonSpacing = 100.f;

	for (int i = 0; i < m_questBoardButtons.size(); i++)
	{
		auto& button = m_questBoardButtons[i];
		button.shape.setSize(sf::Vector2f(buttonWidth, buttonHeight));
		button.shape.setOrigin(button.shape.getSize() / 2.f);
		button.shape.setPosition(sf::Vector2f(buttonStartX, buttonStartY + static_cast<float>(i) * buttonSpacing));

		const sf::FloatRect bounds = button.label.getLocalBounds();

		button.label.setOrigin({
			bounds.position.x + bounds.size.x / 2.f,
			bounds.position.y + bounds.size.y / 2.f
			});
		button.label.setPosition(button.shape.getPosition());
	}

	m_questBoardDivider.setSize(sf::Vector2f(4.f, panelSize.y - 40.f));
	m_questBoardDivider.setOrigin(m_questBoardDivider.getSize() / 2.f);
	m_questBoardDivider.setPosition(sf::Vector2f(viewCenter.x - panelOrigin.x + leftWidth, viewCenter.y));

	m_questBoardTitle.setPosition(sf::Vector2f(viewCenter.x - panelOrigin.x + leftWidth + 36.f, viewCenter.y - panelOrigin.y + 48.f));
	m_questBoardLore.setPosition(sf::Vector2f(viewCenter.x - panelOrigin.x + leftWidth + 36.f, viewCenter.y - panelOrigin.y + 130.f));
	m_questBoardGameplay.setPosition(sf::Vector2f(viewCenter.x - panelOrigin.x + leftWidth + 36.f, viewCenter.y - panelOrigin.y + 260.f));
	m_questBoardReward.setPosition(sf::Vector2f(viewCenter.x - panelOrigin.x + leftWidth + 36.f, viewCenter.y + panelOrigin.y - 110.f));
}

/**
 * @brief Updates quest board button labels from the current quest list.
 */
void MenuUI::updateQuestBoardButtons()
{
	for (int i = 0; i < m_questBoardButtons.size(); i++)
	{
		std::string label;
		if (i < m_questBoardQuests.size() && !m_questBoardQuests[i].title.empty())
			label = m_questBoardQuests[i].title;
		else
			label = "Quest " + std::to_string(i + 1);

		m_questBoardButtons[i].label.setString(label);

		const auto bounds = m_questBoardButtons[i].label.getLocalBounds();
		m_questBoardButtons[i].label.setOrigin({
			bounds.position.x + bounds.size.x / 2.f, 
			bounds.position.y + bounds.size.y / 2.f});
		m_questBoardButtons[i].label.setPosition(m_questBoardButtons[i].shape.getPosition());
	}
}

/**
 * @brief Renders the quest board panel using the current selection.
 * @param window Render target.
 */
void MenuUI::renderQuestBoard(sf::RenderWindow& window)
{
	if (m_questBoardQuests.empty())
		return;

	auto* buttons = getActiveButtons();
	int* selectedIndex = getActiveSelection();
	if (!buttons || !selectedIndex)
		return;

	if (*selectedIndex < 0 || *selectedIndex >= static_cast<int>(m_questBoardQuests.size()))
		*selectedIndex = 0;

	const QuestData& quest = m_questBoardQuests[*selectedIndex];
	m_questBoardTitle.setString(quest.title);
	m_questBoardLore.setString(quest.loreDescription);
	m_questBoardGameplay.setString(quest.buildGameplayDescription());
	m_questBoardReward.setString(quest.buildRewardDescription());

	window.draw(m_questBoardPanel);
	window.draw(m_questBoardDivider);

	for (int i = 0; i < static_cast<int>(buttons->size()); i++)
	{
		auto& button = (*buttons)[i];
		const bool isSelected = (i == *selectedIndex);
		button.shape.setFillColor(isSelected ? sf::Color(60, 60, 60, 240) : sf::Color(30, 30, 30, 220));
		button.shape.setOutlineColor(isSelected ? sf::Color::Yellow : sf::Color::White);
		window.draw(button.shape);
		window.draw(button.label);
	}

	window.draw(m_questBoardTitle);
	window.draw(m_questBoardLore);
	window.draw(m_questBoardGameplay);
	window.draw(m_questBoardReward);
}

/**
 * @brief Applies a quest board selection index with clamping.
 * @param index New selection index.
 */
void MenuUI::applyQuestBoardSelection(int index)
{
	#pragma region Quest System
	m_questBoardSelection = index;
	if (m_questBoardSelection < 0)
		m_questBoardSelection = 0;
	if (m_questBoardSelection >= static_cast<int>(m_questBoardButtons.size()))
		m_questBoardSelection = static_cast<int>(m_questBoardButtons.size()) - 1;
	updateMenuCursorPosition();
	#pragma endregion
}
#pragma endregion

/**
 * @brief Gets the active button list based on the current screen.
 * @return Pointer to button vector, or null when the screen has no buttons.
 */
std::vector<MenuButton>* MenuUI::getActiveButtons()
{
	switch (m_screen)
	{
	case MenuScreen::MAIN_MENU_SCREEN:
		return &m_mainMenuButtons;
	case MenuScreen::PAUSE_MENU_SCREEN:
		return &m_pauseMenuButtons;
	case MenuScreen::GAME_OVER_SCREEN:
		return &m_gameOverButtons;
	case MenuScreen::QUEST_BOARD_SCREEN:
		return &m_questBoardButtons;
	default:
		return nullptr;
	}
}

/**
 * @brief Gets the active selection index pointer for the current screen.
 * @return Pointer to selection integer, or null when the screen has no selection.
 */
int* MenuUI::getActiveSelection()
{
	switch (m_screen)
	{
	case MenuScreen::MAIN_MENU_SCREEN:
		return &m_mainMenuSelection;
	case MenuScreen::PAUSE_MENU_SCREEN:
		return &m_pauseMenuSelection;
	case MenuScreen::GAME_OVER_SCREEN:
		return &m_gameOverSelection;
	case MenuScreen::QUEST_BOARD_SCREEN:
		return &m_questBoardSelection;
	default:
		return nullptr;
	}
}

/**
 * @brief Moves current selection by direction with wrap-around.
 * @param direction Negative to move up, positive to move down.
 */
void MenuUI::moveSelection(int direction)
{
	auto* buttons = getActiveButtons();
	int* selectedIndex = getActiveSelection();
	if (!buttons || !selectedIndex || buttons->empty())
		return;

	const int count = static_cast<int>(buttons->size());
	*selectedIndex = (*selectedIndex + direction + count) % count;
	updateMenuCursorPosition();
}

/**
 * @brief Updates selection based on mouse hover position.
 * @param worldPos Mouse position in world coordinates.
 */
void MenuUI::updateHover(const sf::Vector2f& worldPos)
{
	auto* buttons = getActiveButtons();
	int* selectedIndex = getActiveSelection();
	if (!buttons || !selectedIndex)
		return;

	for (int i = 0; i < static_cast<int>(buttons->size()); i++)
	{
		if ((*buttons)[i].shape.getGlobalBounds().contains(worldPos))
		{
			*selectedIndex = i;
			updateMenuCursorPosition();
			return;
		}
	}
}

/**
 * @brief Sets the pending action from the currently selected button.
 */
void MenuUI::activateSelectedButton()
{
	auto* buttons = getActiveButtons();
	int* selectedIndex = getActiveSelection();
	if (!buttons || !selectedIndex || buttons->empty())
		return;

	m_pendingAction = (*buttons)[*selectedIndex].action;
}

/**
 * @brief Repositions the menu cursor to align with the currently selected button.
 */
void MenuUI::updateMenuCursorPosition()
{
	auto* buttons = getActiveButtons();
	int* selectedIndex = getActiveSelection();
	if (!buttons || !selectedIndex || buttons->empty())
		return;

	if (*selectedIndex < 0 || *selectedIndex >= static_cast<int>(buttons->size()))
		*selectedIndex = 0;

	const sf::Vector2f pos = (*buttons)[*selectedIndex].shape.getPosition();
	const sf::Vector2f size = (*buttons)[*selectedIndex].shape.getSize();
	m_menuCursor.setPosition(sf::Vector2f(pos.x - size.x * 0.5f - 24.f, pos.y));
}

