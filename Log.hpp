#pragma once

#include "PCS.hpp"
#include <Windows.h>

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

	void pErrorMessageBox(std::string title, std::string content); // doesnt have an ownder window since i dont use hwnd
	void pMessageBox(std::string title, std::string content);
	
}