#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "Collision.h"
#include "HubShop.h"

enum NPCPersonality
{
	FRIENDLY,
	GREEDY,
	PASSIONATE,
	PROFESSIONAL,
	RUDE,

	PERSONALITY_COUNT
};

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

	void render(sf::RenderWindow& window);

	const HubNPCInfo& getInfo() const { return npc_info; }
	const std::string& getDialogue() const { return npc_dialogue; }

	sf::FloatRect getCollisionBounds() const override { return npc_interactionBounds; }
	CollisionProfile getCollisionProfile() const override { return npc_collisionProfile; }
	
private:
	void renderInteractionBounds(sf::RenderWindow& window);

	sf::RectangleShape npc_body;
	sf::FloatRect npc_interactionBounds;
	CollisionProfile npc_collisionProfile;

	HubNPCInfo npc_info;
	std::string npc_dialogue;
};

