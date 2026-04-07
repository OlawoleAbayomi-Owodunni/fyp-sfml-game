#pragma once

#include "LLMWrapper.h"

class LLMService
{
public:
	bool init(const std::string& modelPath);
	bool isReady() const;

	std::string generateResponse(const std::string& prompt);

private:
	LLMWrapper llm_wrapper;
	bool llm_isReady{ false };
};

