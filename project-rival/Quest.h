#pragma once

#include <string>
#include <vector>

#include "Enemy.h"
#include "Pickup.h"

#pragma region Quest System

enum class QuestType
{
	KILL,
	FETCH
};

struct QuestData
{
	QuestType type{ QuestType::KILL };
	std::string title{ "Placeholder Quest" };
	std::string loreDescription{ "Placeholder lore description." };
	std::string gameplayDescription{ "Placeholder gameplay description." };
	std::string rewardDescription{ "Reward: 0 coins" };
	int targetCount{ 1 };
	int rewardCoins{ 0 };
	EnemyType enemyType{ EnemyType::GRUNT };
	PickupType pickupType{ PickupType::HEALTH };
	int progress{ 0 };

	bool isComplete() const { return progress >= targetCount; }
	void resetProgress() { progress = 0; }

	std::string buildGameplayDescription() const
	{
		switch (type)
		{
		case QuestType::KILL:
			return "Kill " + std::to_string(targetCount) + " " + getEnemyName(enemyType) + " enemies.";
		case QuestType::FETCH:
			return "Collect " + std::to_string(targetCount) + " " + getPickupName(pickupType) + " packs.";
		default:
			return gameplayDescription;
		}
	}

	std::string buildRewardDescription() const
	{
		return "Reward: " + std::to_string(rewardCoins) + " coins";
	}

	static std::string getEnemyName(EnemyType type)
	{
		switch (type)
		{
		case EnemyType::GRUNT:
			return "Grunt";
		case EnemyType::TURRET:
			return "Turret";
		default:
			return "Enemy";
		}
	}

	static std::string getPickupName(PickupType type)
	{
		switch (type)
		{
		case PickupType::HEALTH:
			return "Health";
		case PickupType::AMMO:
			return "Ammo";
		default:
			return "Pickup";
		}
	}
};

class QuestManager
{
public:
	QuestManager();

	void resetBoard();

	const std::vector<QuestData>& getBoardQuests() const { return m_boardQuests; }
	const QuestData* getBoardQuest(int index) const
	{
		if (index < 0 || index >= static_cast<int>(m_boardQuests.size()))
			return nullptr;

		return &m_boardQuests[index];
	}
    bool updateBoardQuestText(int index, const std::string& title, const std::string& loreDescription);
	bool hasActiveQuest() const { return m_hasActiveQuest; }
	const QuestData* getActiveQuest() const { return m_hasActiveQuest ? &m_activeQuest : nullptr; }
	int getActiveQuestIndex() const { return m_activeQuestIndex; }

	void acceptQuest(int index);

	void recordEnemyKill(EnemyType enemyType);
	void recordPickup(PickupType pickupType);

	void finaliseRun();
	int commitRunResult();

private:
	QuestData generateQuest();
	QuestData generateKillQuest();
	QuestData generateFetchQuest();

	void clearActiveQuestState();


	std::vector<QuestData> m_boardQuests{};
	int m_activeQuestIndex{ -1 };
	bool m_hasActiveQuest{ false };
	bool m_runFinalised{ false };
	bool m_activeQuestCompleted{ false };
	int m_pendingRewardCoins{ 0 };
	QuestData m_activeQuest{};
};

#pragma endregion
