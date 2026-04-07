#include "LLMService.h"

bool LLMService::init(const std::string& modelPath)
{
    llm_isReady = llm_wrapper.LoadModel(modelPath);
	return llm_isReady;
}

bool LLMService::isReady() const
{
    return llm_isReady;
}

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
