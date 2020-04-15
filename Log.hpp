#pragma once

#include "PCS.hpp"

namespace detailEngine
{
	class Log : public Subscriber
	{
	public:
		Log() {}

		void ExecuteMessage(Message message);

	private:
		std::vector<std::string> messageLog;

	};
}