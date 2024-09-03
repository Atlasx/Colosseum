#include "engine.h"

CEngine::CEngine()
{
	std::cout << "Good Morning CEngine" << std::endl;
}

CEngine::~CEngine()
{
	std::cout << "Good Night CEngine" << std::endl;
}

void CEngine::HelloWorld()
{
	std::cout << "CEngine Hello World" << std::endl;
}