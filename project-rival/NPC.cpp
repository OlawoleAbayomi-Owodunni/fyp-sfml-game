#include "NPC.h"

/**
 * @file NPC.cpp
 * @brief Implements hub NPC initialization and debug rendering.
 */

/**
 * @brief Constructs an NPC and initializes render/collision state.
 * @param info Immutable NPC metadata (name, shop type, personality).
 * @param worldPos Initial world position.
 * @param bodyColor Visual color for the NPC body.
 * @param triggerOffset Offset used to place the interaction bounds relative to the body.
 */
NPC::NPC(const HubNPCInfo& info, const sf::Vector2f worldPos, const sf::Color bodyColor, const sf::Vector2f& triggerOffset)
	: npc_info(info)
{
	init(info, worldPos, bodyColor, triggerOffset);
}

/**
 * @brief Initializes the NPC's body, interaction bounds, collision profile, and default dialogue.
 * @param info Immutable NPC metadata (name, shop type, personality).
 * @param worldPos Initial world position.
 * @param bodyColor Visual color for the NPC body.
 * @param triggerOffset Offset used to place the interaction bounds relative to the body.
 */
void NPC::init(const HubNPCInfo& info, const sf::Vector2f worldPos, const sf::Color bodyColor, const sf::Vector2f& triggerOffset)
{
	npc_body.setSize(sf::Vector2f(50.f, 75.f));
	npc_body.setOrigin(npc_body.getSize() / 2.f);
	npc_body.setPosition(worldPos);
	npc_body.setFillColor(bodyColor);

	if (!info.portraitPath.empty())
	{
		if (!npc_sprite.configure(info.portraitPath, {}, sf::Vector2i(0, 0)))
			npc_sprite.configure("ASSETS/SPRITES/UI/Portrait - Petra.png", {}, sf::Vector2i(0, 0));
	}
	else
	{
		npc_sprite.configure("ASSETS/SPRITES/UI/Portrait - Petra.png", {}, sf::Vector2i(0, 0));
	}

	if (npc_sprite.isLoaded())
	{
		const sf::FloatRect spriteBounds = npc_sprite.getGlobalBounds();
		if (spriteBounds.size.x > 0.f && spriteBounds.size.y > 0.f)
		{
			npc_sprite.setOrigin(sf::Vector2f(spriteBounds.size.x * 0.5f, spriteBounds.size.y * 0.5f));
			npc_sprite.setScale(sf::Vector2f(
				npc_body.getSize().x / spriteBounds.size.x,
				npc_body.getSize().y / spriteBounds.size.y));
		}
		npc_sprite.setPosition(npc_body.getPosition());
	}

	const sf::Vector2f triggerSize(100.f, 100.f);
	const sf::Vector2f triggerCenter = worldPos + triggerOffset;
	npc_interactionBounds.position = triggerCenter - triggerSize / 2.f;
	npc_interactionBounds.size = triggerSize;

	npc_collisionProfile.layer = CollisionLayer::NPC_LAYER;
	npc_collisionProfile.mask = CollisionLayer::PLAYER_LAYER;

	switch (info.personality)
	{
	case FRIENDLY: npc_dialogue = "Hey there! Great to see you."; break;
	case GREEDY: npc_dialogue = "Coins first, talk second."; break;
	case PASSIONATE: npc_dialogue = "I live for this craft!"; break;
	case PROFESSIONAL: npc_dialogue = "State your request clearly."; break;
	case RUDE: npc_dialogue = "Make it quick."; break;
	}
}

/**
 * @brief Renders the NPC body and its interaction bounds (debug).
 * @param window Target render window.
 */
void NPC::render(sf::RenderWindow & window, bool texturedMode)
{
	if (texturedMode && npc_sprite.isLoaded())
	{
		npc_sprite.setPosition(npc_body.getPosition());
		npc_sprite.draw(window);
	}
	else
	{
		window.draw(npc_body);
		renderInteractionBounds(window);
	}
}

/**
 * @brief Renders a debug rectangle for the NPC interaction bounds.
 * @param window Target render window.
 */
void NPC::renderInteractionBounds(sf::RenderWindow & window)
{
	sf::RectangleShape debugRect(npc_interactionBounds.size);
	debugRect.setPosition(npc_interactionBounds.position);
	debugRect.setFillColor(sf::Color(0, 255, 0, 100)); // Semi-transparent green
	debugRect.setOutlineThickness(1.f);
	debugRect.setOutlineColor(sf::Color::Green);
	window.draw(debugRect);
}
