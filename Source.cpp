#include "Engine.hpp"

using namespace detailEngine;

int main()
{
	//FreeConsole();
	Engine engine;

	if (engine.Init())
	{
		while (!engine.ShouldClose())
		{
			engine.Update();
		}
	}
	else
	{
		std::cout << "Engine Initialization Failed Successfully." << std::endl;
	}

	engine.Terminate();

	return 0;
}