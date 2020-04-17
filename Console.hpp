#pragma once

#include "Input.hpp"
#include "ECS.hpp"
#include <GLFW/glfw3.h>

namespace detailEngine
{
	class Console : public Publisher , public Subscriber
	{
	public:
		Console() {}

		void Update(Input* input, EntityController* entityController)
		{
			if (input->CheckCombination(256))
			{
				//std::string response = "n";
				//std::cout << "Do you really want to quit ? y/n" << std::endl;
				//std::cin >> response;
				//
				//if (response == "y")
				//{
					pSendMessage(Message(MSG_CONSOLE, std::string("QUIT"), true));
				//}
				
			}
			if (input->CheckCombination(293, 342))
			{
				//std::string response = "n";
				//std::cout << "Do you really want to quit ? y/n" << std::endl;
				//std::cin >> response;
				//
				//if (response == "y")
				//{
					pSendMessage(Message(MSG_CONSOLE, std::string("QUIT"), true));
				//}
			}
			if (input->CheckCombination(69))
			{
				pSendMessage(Message(MSG_ASSET, std::string("Engine Error"), Asset("de_inferno", "models", "obj")));
			}
		}
	};
}