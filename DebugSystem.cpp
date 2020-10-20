#include "DebugSystem.hpp"

namespace detailEngine
{
	void DebugSystem::TestMessageBus(int messageCount)
	{
		for (int i = 0; i < messageCount; ++i)
		{
			pSendMessage(Message(MSG_STRESSTEST, std::string("Debug Test"), std::string("A MESSAGE CONTAINING A LONG STRING")));
		}
	}
}