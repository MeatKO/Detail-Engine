#include "Log.hpp"

namespace detailEngine
{
	// Dont forget to add new message types to the supported topic list of the Log ( Check Engine.hpp )
	void Log::ExecuteMessage(Message message)
	{
		if (message.GetTopic() == MSG_LOG)
		{
			std::string messageString = std::any_cast<std::string>(message.GetEvent()) + " : " + std::any_cast<std::string>(message.GetValue());
			std::cout << messageString << std::endl;
			//messageLog.push_back(messageString);
		}
		else if (message.GetTopic() == MSG_KEY)
		{
			std::string messageString = std::any_cast<std::string>(message.GetEvent()) + " : " + std::to_string(std::any_cast<int>(message.GetValue()));
			//std::cout << messageString << std::endl;
			//messageLog.push_back(messageString);
		}
		else if (message.GetTopic() == MSG_ERROR_MESSAGE)
		{
			pErrorMessageBox(std::any_cast<std::string>(message.GetEvent()), std::any_cast<std::string>(message.GetValue()));
		}
		else if (message.GetTopic() == MSG_MESSAGE_BOX)
		{
			pMessageBox(std::any_cast<std::string>(message.GetEvent()), std::any_cast<std::string>(message.GetValue()));
		}
	}

	void pErrorMessageBox(std::string title, std::string content)
	{
		MessageBox(NULL, title.c_str(), content.c_str(), MB_OK | MB_ICONWARNING);
	}

	void pMessageBox(std::string title, std::string content)
	{
		MessageBox(NULL, title.c_str(), content.c_str(), MB_OK);
	}
}
