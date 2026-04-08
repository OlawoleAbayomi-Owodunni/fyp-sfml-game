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
	bool initAsync(const std::string& modelPath);
	std::optional<bool> tryConsumeInitResult();

	bool isReady() const;
	bool isInitInProgress() const;
	bool isBusy() const;

	bool requestGenerate(const std::string& prompt);
	std::optional<std::string> tryConsumeLatestResponse();


	void shutdown();

private:
	void initWorkerMain(const std::string& modelPath);
	void generateWorkerMain(std::string prompt);


	LLMWrapper llm_wrapper;
	
	std::atomic<bool> llm_isReady{ false };
	
	// Initialization management
	std::atomic<bool> llm_initInProgress{ false };
	std::thread llm_initWorker;

	std::mutex llm_initMutex;
	std::optional<bool> llm_initResult;

	// Generation worker management
	std::atomic<bool> llm_isBusy{ false };
	std::thread llm_generateWorker;
	
	std::mutex llm_resultMutex;
	std::optional<std::string> llm_latestResponse;
};

