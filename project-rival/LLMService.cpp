#include "LLMService.h"

/// <summary>
/// Initializes the LLM service by loading a model from the specified path.
/// </summary>
/// <param name="modelPath">The file path to the model to be loaded.</param>
/// <returns>True if the model was loaded successfully and the service is ready; otherwise, false.</returns>
bool LLMService::init(const std::string& modelPath)
{
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

/// <summary>
/// Generates a response from the LLM based on the provided prompt.
/// </summary>
/// <param name="prompt">The input prompt to send to the language model.</param>
/// <returns>The generated response from the LLM, or an error message if the model is not ready.</returns>
std::string LLMService::generateResponse(const std::string& prompt)
{
    if(llm_isReady)
    {
        return llm_wrapper.Generate(prompt);
    }
    else
    {
        return "LLM model did not Load properly.";
	}
}
