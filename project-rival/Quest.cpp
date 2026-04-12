#include "Quest.h"

QuestManager::QuestManager()
{
	resetBoard();
}

void QuestManager::resetBoard()
{
	m_boardQuests.clear();
	m_boardQuests.reserve(3);
	for (int i = 0; i < 3; ++i)
		m_boardQuests.push_back(generateQuest());

	clearActiveQuestState();
}

void QuestManager::acceptQuest(int index)
{
	if (index < 0 || index >= m_boardQuests.size())
		return;

	if (m_hasActiveQuest)
		return;

	m_activeQuestIndex = index;
	m_activeQuest = m_boardQuests[index];
	m_activeQuest.resetProgress();
	m_hasActiveQuest = true;
	m_runFinalised = false;
	m_activeQuestCompleted = false;
	m_pendingRewardCoins = 0;
}

bool QuestManager::updateBoardQuestText(int index, const std::string& title, const std::string& loreDescription)
{
	if (index < 0 || index >= static_cast<int>(m_boardQuests.size()))
		return false;

	if (!title.empty())
		m_boardQuests[index].title = title;

	if (!loreDescription.empty())
		m_boardQuests[index].loreDescription = loreDescription;

	return true;
}

void QuestManager::recordEnemyKill(EnemyType enemyType)
{
	if (!m_hasActiveQuest || m_runFinalised)
		return;

	if (m_activeQuest.type != QuestType::KILL)
		return;

	if (m_activeQuest.enemyType != enemyType)
		return;

	m_activeQuest.progress += 1;
}

void QuestManager::recordPickup(PickupType pickupType)
{
	if (!m_hasActiveQuest || m_runFinalised)
		return;

	if (m_activeQuest.type != QuestType::FETCH)
		return;

	if (m_activeQuest.pickupType != pickupType)
		return;

	m_activeQuest.progress += 1;
}

void QuestManager::finaliseRun()
{
	m_runFinalised = true;
	m_activeQuestCompleted = m_hasActiveQuest && m_activeQuest.isComplete();
	if (m_activeQuestCompleted)
		m_pendingRewardCoins = m_activeQuest.rewardCoins;
	else
		m_pendingRewardCoins = 0;
}

int QuestManager::commitRunResult()
{
	int reward = 0;

	if (m_runFinalised && m_hasActiveQuest)
	{
		if (m_activeQuestCompleted && m_activeQuestIndex >= 0 && m_activeQuestIndex < m_boardQuests.size())
		{
			reward = m_pendingRewardCoins;
			m_boardQuests[m_activeQuestIndex] = generateQuest();
		}
	}

	clearActiveQuestState();
	return reward;
}


QuestData QuestManager::generateQuest()
{
	const QuestType type = (std::rand() % 2 == 0) ? QuestType::KILL : QuestType::FETCH;
	return (type == QuestType::KILL) ? generateKillQuest() : generateFetchQuest();
}

QuestData QuestManager::generateKillQuest()	// we would plug in LLM Generation in here
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

QuestData QuestManager::generateFetchQuest()
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

void QuestManager::clearActiveQuestState()
{
	m_activeQuestIndex = -1;
	m_hasActiveQuest = false;
	m_runFinalised = false;
	m_activeQuestCompleted = false;
	m_pendingRewardCoins = 0;
	m_activeQuest = QuestData{};
}

