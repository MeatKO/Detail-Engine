#pragma once

#include "PCS.hpp"
#include <Windows.h>

namespace detailEngine
{
	struct Key
	{
		bool isPressed = false;
		double lastTime = 0.0f;
	};

	class Input : public Publisher
	{
	public:
		Input()
		{
			for (int i = 0; i <= 1024; i++)
			{
				keyList.push_back(Key());
			}
		}
		void SetKey(int KeyID, double CurrentTime)
		{
			if (KeyID >= 0 && KeyID <= 1024)
			{
				keyList[KeyID].isPressed = true;
				keyList[KeyID].lastTime = CurrentTime;
				pSendMessage(Message(MSG_KEY, std::string("PRESS"), int(KeyID)));
			}
		}
		void UnsetKey(int KeyID, double CurrentTime)
		{
			if (KeyID >= 0 && KeyID <= 1024)
			{
				if ((CurrentTime - keyList[KeyID].lastTime) >= holdDeltaTime)
				{
					pSendMessage(Message(MSG_KEY, std::string("HOLD_RELEASE"), int(KeyID)));
				}
				else
				{
					pSendMessage(Message(MSG_KEY, std::string("RELEASE"), int(KeyID)));
				}
				keyList[KeyID].isPressed = false;
			}
		}
		bool IsCapsOn()
		{
			return capsLock;
		}
		bool IsPressed(int KeyID)
		{
			if (KeyID >= 0 && KeyID <= 1024)
			{
				return keyList[KeyID].isPressed;
			}
		}
		bool IsHeld(int KeyID, double CurrentTime)
		{
			if (KeyID >= 0 && KeyID <= 1024)
			{
				if (keyList[KeyID].isPressed && ((CurrentTime - keyList[KeyID].lastTime) >= holdDeltaTime))
				{
					return true;
				}
			}
			return false;
		}
		double GetKeyDeltaTime(int KeyID)
		{
			if (KeyID >= 0 && KeyID <= 1024)
			{
				return keyList[KeyID].lastTime;
			}
		}
		void Update(double CurrentTime)
		{
			bool shift = (GetKeyState(VK_SHIFT) & 0x0100);

			if (shift ^ (!GetKeyState(VK_CAPITAL) & 0x0001))
			{
				capsLock = true;
			}
			else
			{
				capsLock = false;
			}

			for (int loopKey = 1; loopKey <= 1024; loopKey++)
			{
				//int capitalOffset = 0;

				//if (loopKey >= 65 && loopKey <= 90 && (((GetKeyState(VK_SHIFT) & 0x8000) || (GetKeyState(VK_CAPITAL) & 0x0001)) != 0))
				//{
				//	// 'A' and 'a' are offset by 32 so just add 32 if Shift is up and Caps Lock is not active
				//	capitalOffset = 32;
				//}

				if (GetKeyState(loopKey) & 0x0100)
				{
					
					if (IsPressed(loopKey))
					{
						if (IsHeld(loopKey, CurrentTime))
						{
							pSendMessage(Message(MSG_KEY, std::string("HOLD"), int(loopKey)));
						}
					}
					else
					{
						//SetKey(loopKey, CurrentTime);
					}
				}
				else
				{
					if (IsPressed(loopKey))
					{
						//UnsetKey(loopKey, CurrentTime);
					}
				}
			}
		}

		double holdDeltaTime = 1.0f; // in seconds
		bool capsLock = false;
		std::vector<Key> keyList;
	};
}