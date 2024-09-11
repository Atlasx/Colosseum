#include <iostream>

namespace CE
{
	enum CEResult {
		Failed = 0,
		Success = 1
	};

	class Engine
	{
	public:
		Engine();
		~Engine();

		CEResult Start();

	private:
	};

}