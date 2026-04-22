#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "Collision.h"
#include "HubShop.h"
#include "CustomSprite.h"

/**
 * @file NPC.h
 * @brief Declares the hub NPC type and related metadata.
 */

/**
 * @brief Dialogue/behaviour personality of an NPC.
 */
enum NPCPersonality
{
	FRIENDLY,
	GREEDY,
	PASSIONATE,
	PROFESSIONAL,
	RUDE,

	PERSONALITY_COUNT
};

/**
 * @brief Static metadata used to initialise a hub NPC.
 */
struct HubNPCInfo
{
	std::string name;
	HubShopType shopType;
	NPCPersonality personality;
	std::string background;
};

class NPC : public ICollidable
{
public:
	NPC(const HubNPCInfo& info,
		const sf::Vector2f worldPos, 
		const sf::Color bodyColor,
		const sf::Vector2f& triggerOffset);

	void init(const HubNPCInfo& info,
		const sf::Vector2f worldPos,
		const sf::Color bodyColor,
		const sf::Vector2f& triggerOffset);

	void render(sf::RenderWindow& window, bool texturedMode);

	const HubNPCInfo& getInfo() const { return npc_info; }
	const std::string& getDialogue() const { return npc_dialogue; }

	sf::FloatRect getCollisionBounds() const override { return npc_interactionBounds; }
	CollisionProfile getCollisionProfile() const override { return npc_collisionProfile; }
	
private:
	void renderInteractionBounds(sf::RenderWindow& window);

	sf::RectangleShape npc_body;
	CustomSprite npc_sprite;
	sf::FloatRect npc_interactionBounds;
	CollisionProfile npc_collisionProfile;

	HubNPCInfo npc_info;
	std::string npc_dialogue;
};

