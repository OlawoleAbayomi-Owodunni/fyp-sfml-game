#pragma once
#include "LLMWrapper.h"

#include <atomic>
#include <thread>
#include <mutex>
#include <optional>

class LLMService
{
public:
	LLMService() = default;
	~LLMService();

	bool init(const std::string& modelPath);
	bool isReady() const;

	bool requestGenerate(const std::string& prompt);

	//std::string generateResponse(const std::string& prompt);
	std::optional<std::string> tryConsumeLatestResponse();

	bool isBusy() const;

	void shutdown();

private:
	void workerMain(std::string prompt);


	LLMWrapper llm_wrapper;
	bool llm_isReady{ false };

	std::atomic<bool> llm_isBusy{ false };

	std::thread llm_worker;
	std::mutex llm_resultMutex;
	std::optional<std::string> llm_latestResponse;
};

