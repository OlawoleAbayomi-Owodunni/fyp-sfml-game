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
};

class NPC : public ICollidable
{
public:
	NPC(const HubNPCInfo& info);

	sf::FloatRect getCollisionBounds() const override { return npc_interactionBounds; }
	CollisionProfile getCollisionProfile() const override { return npc_collisionProfile; }


	
private:
	sf::RectangleShape npc_body;
	sf::FloatRect npc_interactionBounds;
	CollisionProfile npc_collisionProfile;

	HubNPCInfo npc_info;
	std::string npc_dialogue;
};

