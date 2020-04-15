#include "Log.hpp"

namespace detailEngine
{
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
			std::cout << messageString << std::endl;
			//messageLog.push_back(messageString);
		}
	}
}
