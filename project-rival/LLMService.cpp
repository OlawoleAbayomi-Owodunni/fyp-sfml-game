#include "LLMService.h"

/**
 * @file LLMService.cpp
 * @brief Implements async model initialization and prompt generation work.
 */

/**
 * @brief Ensures background workers are joined and the model is unloaded.
 */
LLMService::~LLMService()
{
    shutdown();
}

/**
 * @brief Initializes the service synchronously by loading a model.
 * @param modelPath Model file path.
 * @return True when the model was loaded successfully.
 */
bool LLMService::init(const std::string& modelPath)
{
	shutdown(); // Ensure any existing model is unloaded before initializing a new one.
    llm_isReady = llm_wrapper.LoadModel(modelPath);
	return llm_isReady;
}

/**
 * @brief Starts asynchronous model initialization.
 *
 * If initialization is already in progress or the service is already ready, the
 * request is rejected.
 *
 * @param modelPath Model file path.
 * @return True when the worker thread was started.
 */
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


/**
 * @brief Indicates whether the service has a loaded model and can accept work.
 */
bool LLMService::isReady() const
{
    return llm_isReady.load();
}

/**
 * @brief Indicates whether async initialization is currently running.
 */
bool LLMService::isInitInProgress() const
{
	return llm_initInProgress.load();
}

/**
 * @brief Indicates whether a generation request is currently running.
 */
bool LLMService::isBusy() const
{
    return llm_isBusy.load();
}


/**
 * @brief Requests a single prompt generation on a background thread.
 *
 * Returns false when the service is not ready or already busy.
 *
 * @param prompt Prompt text to send to the model.
 * @return True when the request was accepted.
 */
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


/**
 * @brief Consumes the result of async initialization if available.
 * @return Optional bool containing the init result; empty when no result is available.
 */
std::optional<bool> LLMService::tryConsumeInitResult()
{
	std::lock_guard<std::mutex> lock(llm_initMutex);
	if (!llm_initResult.has_value())
		return std::nullopt; // No result available

	auto result = llm_initResult;
	llm_initResult.reset(); // Clear the result after consuming
	return result;
}

/**
 * @brief Consumes the latest generated response if available.
 *
 * Consuming a response also clears the stored response and marks the service as
 * no longer busy.
 *
 * @return Optional response string; empty when no response is available.
 */
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


/**
 * @brief Background worker entrypoint for asynchronous initialization.
 * @param modelPath Model file path.
 */
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

/**
 * @brief Background worker entrypoint for asynchronous generation.
 * @param prompt Prompt text to generate from.
 */
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


/**
 * @brief Stops background workers, clears cached results, and unloads the model.
 */
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