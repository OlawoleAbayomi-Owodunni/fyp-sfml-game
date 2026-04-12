#pragma once

#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>

#include "Enemy.h"
#include "Pickup.h"

#pragma region LLM Generated

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
	QuestManager()
	{
		resetBoard();
	}

	void resetBoard()
	{
		m_boardQuests.clear();
		m_boardQuests.reserve(3);
		for (int i = 0; i < 3; ++i)
			m_boardQuests.push_back(generateQuest());

		clearActiveQuestState();
	}

	const std::vector<QuestData>& getBoardQuests() const { return m_boardQuests; }

	const QuestData* getBoardQuest(int index) const
	{
		if (index < 0 || index >= static_cast<int>(m_boardQuests.size()))
			return nullptr;

		return &m_boardQuests[index];
	}

	bool hasActiveQuest() const { return m_hasActiveQuest; }

	const QuestData* getActiveQuest() const
	{
		return m_hasActiveQuest ? &m_activeQuest : nullptr;
	}

	int getActiveQuestIndex() const { return m_activeQuestIndex; }

	void acceptQuest(int index)
	{
		if (index < 0 || index >= static_cast<int>(m_boardQuests.size()))
			return;

		if (m_hasActiveQuest)
			return;

		m_activeQuestIndex = index;
		m_activeQuest = m_boardQuests[index];
		m_activeQuest.resetProgress();
		m_hasActiveQuest = true;
		m_runFinalized = false;
		m_activeQuestCompleted = false;
		m_pendingRewardCoins = 0;
	}

	void recordEnemyKill(EnemyType enemyType)
	{
		if (!m_hasActiveQuest || m_runFinalized)
			return;

		if (m_activeQuest.type != QuestType::KILL)
			return;

		if (m_activeQuest.enemyType != enemyType)
			return;

		m_activeQuest.progress = std::min(m_activeQuest.progress + 1, m_activeQuest.targetCount);
	}

	void recordPickup(PickupType pickupType)
	{
		if (!m_hasActiveQuest || m_runFinalized)
			return;

		if (m_activeQuest.type != QuestType::FETCH)
			return;

		if (m_activeQuest.pickupType != pickupType)
			return;

		if (pickupType != PickupType::HEALTH && pickupType != PickupType::AMMO)
			return;

		m_activeQuest.progress = std::min(m_activeQuest.progress + 1, m_activeQuest.targetCount);
	}

	void finalizeRun()
	{
		m_runFinalized = true;
		m_activeQuestCompleted = m_hasActiveQuest && m_activeQuest.isComplete();
		m_pendingRewardCoins = m_activeQuestCompleted ? m_activeQuest.rewardCoins : 0;
	}

	int commitRunResult()
	{
		int reward = 0;

		if (m_runFinalized && m_hasActiveQuest)
		{
			if (m_activeQuestCompleted && m_activeQuestIndex >= 0 && m_activeQuestIndex < static_cast<int>(m_boardQuests.size()))
			{
				reward = m_pendingRewardCoins;
				m_boardQuests[m_activeQuestIndex] = generateQuest();
			}
		}

		clearActiveQuestState();
		return reward;
	}

private:
	QuestData generateQuest()
	{
		const QuestType type = (std::rand() % 2 == 0) ? QuestType::KILL : QuestType::FETCH;
		return (type == QuestType::KILL) ? generateKillQuest() : generateFetchQuest();
	}

	QuestData generateKillQuest()
	{
		static const std::vector<std::string> titles{
			"Ash Contract",
			"Clearance Order",
			"Hunter Work"
		};

		static const std::vector<std::string> lore{
			"The hub needs one less gang problem in the tunnels.",
			"Someone wants the local threat reduced before the next run.",
			"A simple contract for anyone willing to clean house."
		};

		QuestData quest;
		quest.type = QuestType::KILL;
		quest.enemyType = (std::rand() % 2 == 0) ? EnemyType::GRUNT : EnemyType::TURRET;
		quest.targetCount = 3 + (std::rand() % 4);
		quest.rewardCoins = 1000;
		quest.title = titles[std::rand() % titles.size()];
		quest.loreDescription = lore[std::rand() % lore.size()];
		quest.gameplayDescription = quest.buildGameplayDescription();
		quest.rewardDescription = quest.buildRewardDescription();
		return quest;
	}

	QuestData generateFetchQuest()
	{
		static const std::vector<std::string> titles{
			"Supply Run",
			"Field Recovery",
			"Cache Sweep"
		};

		static const std::vector<std::string> lore{
			"The med bay and armory are running low on supplies.",
			"Recover whatever can be brought back from the field.",
			"Useful packs tend to disappear in the maze below."
		};

		QuestData quest;
		quest.type = QuestType::FETCH;
		quest.pickupType = (std::rand() % 2 == 0) ? PickupType::HEALTH : PickupType::AMMO;
		quest.targetCount = 2 + (std::rand() % 4);
		quest.rewardCoins = 500;
		quest.title = titles[std::rand() % titles.size()];
		quest.loreDescription = lore[std::rand() % lore.size()];
		quest.gameplayDescription = quest.buildGameplayDescription();
		quest.rewardDescription = quest.buildRewardDescription();
		return quest;
	}

	void clearActiveQuestState()
	{
		m_activeQuestIndex = -1;
		m_hasActiveQuest = false;
		m_runFinalized = false;
		m_activeQuestCompleted = false;
		m_pendingRewardCoins = 0;
		m_activeQuest = QuestData{};
	}

	std::vector<QuestData> m_boardQuests{};
	int m_activeQuestIndex{ -1 };
	bool m_hasActiveQuest{ false };
	bool m_runFinalized{ false };
	bool m_activeQuestCompleted{ false };
	int m_pendingRewardCoins{ 0 };
	QuestData m_activeQuest{};
};

#pragma endregion
