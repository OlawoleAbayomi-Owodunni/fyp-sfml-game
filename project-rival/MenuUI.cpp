#include "MenuUI.h"

void MenuUI::init(const sf::Font& font, const sf::View& defaultView)
{
	m_font = &font;
	m_screen = MenuScreen::MAIN_MENU_SCREEN;
	m_mainMenuSelection = 0;
	m_pauseMenuSelection = 0;
	m_gameOverSelection = 0;
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
			button.shape.setFillColor(sf::Color(30, 30, 30, 220));
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
		makeButton("Go To Hub World", MenuAction::ACTION_HUB_WORLD),
		makeButton("Go To Main Menu", MenuAction::ACTION_MAIN_MENU)
	};

	m_gameOverButtons = {
		makeButton("Go To Hub World", MenuAction::ACTION_HUB_WORLD)
	};

	layoutMenu(m_mainMenuButtons, defaultView, 360.f);
	layoutMenu(m_pauseMenuButtons, defaultView, 360.f);
	layoutMenu(m_gameOverButtons, defaultView, 390.f);
	updateMenuCursorPosition();
}

void MenuUI::setScreen(MenuScreen screen)
{
	m_screen = screen;
	updateMenuCursorPosition();
}

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
			if (m_screen == MenuScreen::PAUSE_MENU_SCREEN)
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

void MenuUI::processControllerNavigation(bool upPressed, bool downPressed, bool activatePressed)
{
	if (upPressed)
		moveSelection(-1);
	if (downPressed)
		moveSelection(1);
	if (activatePressed)
		activateSelectedButton();
}

MenuAction MenuUI::consumeAction()
{
	const MenuAction action = m_pendingAction;
	m_pendingAction = MenuAction::ACTION_NONE;
	return action;
}

void MenuUI::render(sf::RenderWindow& window)
{
	auto* buttons = getActiveButtons();
	int* selectedIndex = getActiveSelection();
	if (!buttons || !selectedIndex || !m_font)
		return;

	const sf::View defaultView = window.getDefaultView();
	const sf::Vector2f viewCenter = defaultView.getCenter();
	const sf::Vector2f viewSize = defaultView.getSize();

	sf::RectangleShape overlay(viewSize);
	//set the overlay to solid black if in main menu and then to semi-transparent black if in pause or game over menu
	if (m_screen == MenuScreen::MAIN_MENU_SCREEN)
		overlay.setFillColor(sf::Color::Black);
	else
		overlay.setFillColor(sf::Color(0, 0, 0, 170));
	overlay.setOrigin(viewSize * 0.5f);
	overlay.setPosition(viewCenter);
	window.draw(overlay);

	sf::Text title(*m_font);
	title.setCharacterSize(48);
	title.setFillColor(sf::Color::White);

	if (m_screen == MenuScreen::MAIN_MENU_SCREEN)
		title.setString("Space Raider");
	else if (m_screen == MenuScreen::PAUSE_MENU_SCREEN)
		title.setString("Paused");
	else
		title.setString("Game Over");

	title.setOrigin(title.getGlobalBounds().getCenter());
	title.setPosition(sf::Vector2f(viewCenter.x, viewCenter.y - 210.f));
	window.draw(title);

	for (int i = 0; i < static_cast<int>(buttons->size()); i++)
	{
		auto& button = (*buttons)[i];
		const bool isSelected = (i == *selectedIndex);
		button.shape.setFillColor(isSelected ? sf::Color(60, 60, 60, 240) : sf::Color(30, 30, 30, 220));
		button.shape.setOutlineColor(isSelected ? sf::Color::Yellow : sf::Color::White);
		window.draw(button.shape);
		window.draw(button.label);
	}

	updateMenuCursorPosition();
	window.draw(m_menuCursor);
}

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
	default:
		return nullptr;
	}
}

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
	default:
		return nullptr;
	}
}

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

void MenuUI::activateSelectedButton()
{
	auto* buttons = getActiveButtons();
	int* selectedIndex = getActiveSelection();
	if (!buttons || !selectedIndex || buttons->empty())
		return;

	m_pendingAction = (*buttons)[*selectedIndex].action;
}

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

