#include "NPC.h"

NPC::NPC(const HubNPCInfo& info, const sf::Vector2f worldPos, const sf::Color bodyColor, const sf::Vector2f& triggerOffset)
	: npc_info(info)
{
	init(info, worldPos, bodyColor, triggerOffset);
}

void NPC::init(const HubNPCInfo& info, const sf::Vector2f worldPos, const sf::Color bodyColor, const sf::Vector2f& triggerOffset)
{
	npc_body.setSize(sf::Vector2f(50.f, 75.f));
	npc_body.setOrigin(npc_body.getSize() / 2.f);
	npc_body.setPosition(worldPos);
	npc_body.setFillColor(bodyColor);

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

void NPC::render(sf::RenderWindow & window)
{
	window.draw(npc_body);
	renderInteractionBounds(window);
}

void NPC::renderInteractionBounds(sf::RenderWindow & window)
{
	sf::RectangleShape debugRect(npc_interactionBounds.size);
	debugRect.setPosition(npc_interactionBounds.position);
	debugRect.setFillColor(sf::Color(0, 255, 0, 100)); // Semi-transparent green
	debugRect.setOutlineThickness(1.f);
	debugRect.setOutlineColor(sf::Color::Green);
	window.draw(debugRect);
}
