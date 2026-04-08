#include "LLMService.h"

LLMService::~LLMService()
{
    shutdown();
}

/// <summary>
/// Initializes the LLM service by loading a model from the specified path.
/// </summary>
/// <param name="modelPath">The file path to the model to be loaded.</param>
/// <returns>True if the model was loaded successfully and the service is ready; otherwise, false.</returns>
bool LLMService::init(const std::string& modelPath)
{
	shutdown(); // Ensure any existing model is unloaded before initializing a new one.
    llm_isReady = llm_wrapper.LoadModel(modelPath);
	return llm_isReady;
}

bool LLMService::initAsync(const std::string& modelPath)
{
	// If the LLM is already ready or initialization is currently in progress, return false
	if (llm_isReady || llm_initInProgress)
		return false;

	// If there's an existing initialization worker thread, join it before starting a new one
	if (llm_initWorker.joinable())
		llm_initWorker.join();

	// Clear any previous initialization result before starting a new initialization process
	{
		std::lock_guard<std::mutex> lock(llm_initMutex);
		llm_initResult.reset();
	}
	llm_initInProgress = true;
	llm_initWorker = std::thread(&LLMService::initWorkerMain, this, modelPath);
	return true;
}


/// <summary>
/// Checks whether the LLM service is ready.
/// </summary>
/// <returns>True if the LLM service is ready; otherwise, false.</returns>
bool LLMService::isReady() const
{
    return llm_isReady.load();
}

bool LLMService::isInitInProgress() const
{
	return llm_initInProgress.load();
}

bool LLMService::isBusy() const
{
    return llm_isBusy.load();
}


bool LLMService::requestGenerate(const std::string& prompt)
{
	// If the LLM is not ready or is currently busy processing another request, return false
    if (!llm_isReady || llm_isBusy)
        return false;

	// If there's an existing worker thread, join it before starting a new one
    if (llm_generateWorker.joinable())
        llm_generateWorker.join();

	// Start a new worker thread to process the generation request
    {
		std::lock_guard<std::mutex> lock(llm_resultMutex);
		llm_latestResponse.reset(); // Clear any previous response
    }
    llm_generateWorker = std::thread(&LLMService::generateWorkerMain, this, prompt);

	llm_isBusy = true;
	return true;
}


std::optional<bool> LLMService::tryConsumeInitResult()
{
	std::lock_guard<std::mutex> lock(llm_initMutex);
	if (!llm_initResult.has_value())
		return std::nullopt; // No result available

	auto result = llm_initResult;
	llm_initResult.reset(); // Clear the result after consuming
	return result;
}

std::optional<std::string> LLMService::tryConsumeLatestResponse()
{
	std::lock_guard<std::mutex> lock(llm_resultMutex);

	if (!llm_latestResponse.has_value())
		return std::nullopt; // No response available

	// Move the response out of the optional and clear it
	std::optional<std::string> response = std::move(llm_latestResponse);
	llm_latestResponse.reset();

	llm_isBusy = false;
	return response;
}


void LLMService::initWorkerMain(const std::string& modelPath)
{	
	llm_wrapper.UnloadModel(); // Ensure any existing model is unloaded before loading a new one

	llm_isReady = llm_wrapper.LoadModel(modelPath);
	{
		std::lock_guard<std::mutex> lock(llm_initMutex);
		llm_initResult = llm_isReady; // Store the result of the initialization
	}
	llm_initInProgress = false; // Mark initialization as complete
}

void LLMService::generateWorkerMain(std::string prompt)
{
	// Generate the response using the LLM wrapper
    std::string response = llm_wrapper.Generate(prompt);
    {
        std::lock_guard<std::mutex> lock(llm_resultMutex);
		llm_latestResponse = response; // Store the generated response
    }

	llm_isBusy = false; // Mark the Manager as no longer busy
}


void LLMService::shutdown()
{
	if (llm_initWorker.joinable())
		llm_initWorker.join(); // Ensure any initialization worker thread is finished before shutting down

	if(llm_generateWorker.joinable())
		llm_generateWorker.join(); // Ensure any worker thread is finished before shutting down

	llm_initInProgress = false; // Reset initialization state
	llm_isBusy = false; // Reset busy state
	llm_isReady = false; // Mark the LLM as not ready

	{
		std::lock_guard<std::mutex> lock(llm_initMutex);
		llm_initResult.reset(); // Clear any stored initialization result
	}

	{
		std::lock_guard<std::mutex> lock(llm_resultMutex);
		llm_latestResponse.reset(); // Clear any stored response
	}

	llm_wrapper.UnloadModel(); // Unload the model to free resources
}