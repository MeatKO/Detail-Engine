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
			//capsLock = input->IsCapsOn();
			//
			////Print();
			//
			//if (input->IsPressed(256))
			//{
			//	pSendMessage(Message(MSG_CONSOLE, std::string("QUIT"), true));
			//	
			//}
		}

		void ExecuteMessage(Message message)
		{
			if (message.GetTopic() == MSG_KEY)
			{
				if (message.GetTopic() == MSG_KEY)
				{
					if(std::any_cast<std::string>(message.GetEvent()) == "PRESS")
					{
						if (std::any_cast<int>(message.GetValue()) == 256)
						{
							pSendMessage(Message(MSG_CONSOLE, std::string("QUIT"), true));
						}
					}
				}

				//int newCharacter = std::any_cast<int>(message.GetValue());
				//std::string event = std::any_cast<std::string>(message.GetEvent());
				//
				//if (event == "PRESS" || event == "HOLD")
				//{
				//	if (newCharacter >= 0 && newCharacter <= 256)
				//	{
				//		if (focused)
				//		{
				//			if (newCharacter >= 65 && newCharacter <= 90)
				//			{
				//				if (capsLock)
				//				{
				//					AddCharacter(newCharacter + 32);
				//				}
				//				else
				//				{
				//					AddCharacter(newCharacter);
				//				}
				//			}
				//			else
				//			{
				//				AddCharacter(newCharacter);
				//			}
				//		}
				//	}
				//	else if (newCharacter == '`')
				//	{
				//		ToggleFocus();
				//	}
				//	else if (newCharacter == '256')
				//	{
				//		commandList.push_back(currentCommand);
				//		currentCommand.clear();
				//	}
				//}
				

			}
		}

		void Print()
		{
			std::lock_guard<std::mutex> mut(commandMutex);
			system("CLS");
			std::cout << ">";
			std::cout << currentCommand;
			std::cout << "_" << std::endl;
		}

		void AddCharacter(char character)
		{
			currentCommand += character;
		}

	private:
		std::mutex commandMutex;
		std::string currentCommand = "";
		std::string currentDir = "";
		std::vector<std::string> commandList;
		bool focused = false;
		bool capsLock = false;

		void ToggleFocus()
		{
			//if (!focused)
			//{
			//	if (currentCommand.size() > 0)
			//	{
			//		commandList.push_back(currentCommand);
			//		currentCommand.clear();
			//	}
			//}
			std::cout << "toggled : " << focused << std::endl;
			focused = !focused;
		}
	};
}