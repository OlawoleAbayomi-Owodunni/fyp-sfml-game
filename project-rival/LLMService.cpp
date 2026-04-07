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

/// <summary>
/// Checks whether the LLM service is ready.
/// </summary>
/// <returns>True if the LLM service is ready; otherwise, false.</returns>
bool LLMService::isReady() const
{
    return llm_isReady;
}

bool LLMService::isBusy() const
{
    return llm_isBusy.load();
}

bool LLMService::requestGenerate(const std::string& prompt)
{
	// If the LLM is not ready or is currently busy processing another request, return false
    if (!llm_isReady || llm_isBusy.exchange(true))
        return false;

	// If there's an existing worker thread, join it before starting a new one
    if (llm_worker.joinable())
        llm_worker.join();

	// Start a new worker thread to process the generation request
    {
		std::lock_guard<std::mutex> lock(llm_resultMutex);
		llm_latestResponse.reset(); // Clear any previous response
    }
    llm_worker = std::thread(&LLMService::workerMain, this, prompt);
	return true;
}

void LLMService::workerMain(std::string prompt)
{
	// Generate the response using the LLM wrapper
    std::string response = llm_wrapper.Generate(prompt);
    {
        std::lock_guard<std::mutex> lock(llm_resultMutex);
		llm_latestResponse = response; // Store the generated response
    }

	llm_isBusy.store(false); // Mark the LLM as no longer busy
}

std::optional<std::string> LLMService::tryConsumeLatestResponse()
{
	std::lock_guard<std::mutex> lock(llm_resultMutex);

    if(!llm_latestResponse.has_value())
		return std::nullopt; // No response available

	// Move the response out of the optional and clear it
	std::optional<std::string> response = std::move(llm_latestResponse);
	llm_latestResponse.reset();
	return response;
}


void LLMService::shutdown()
{
	if(llm_worker.joinable())
		llm_worker.join(); // Ensure any worker thread is finished before shutting down

	llm_isBusy.store(false); // Reset busy state

	{
		std::lock_guard<std::mutex> lock(llm_resultMutex);
		llm_latestResponse.reset(); // Clear any stored response
	}
}


