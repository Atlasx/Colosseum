#pragma once

#include "Systems/EngineSystem.h"

namespace CE
{
	class ResourceSystem : public CEngineSystem
	{
	public:
		
		std::vector<std::string> GetDependencies() const override;

		~ResourceSystem() override;
	
	protected:

		void Startup() override;
		void Shutdown() override;
	};

}