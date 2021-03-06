#pragma once

#include <vector>
#include <tuple>
#include <string>
#include <any>
#include <typeinfo>
#include <functional>
#include <queue>
#include <memory>
#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>

namespace detailEngine
{
	class Message;
	class Publisher;
	class Channel;
	class Subscriber;

	enum MessageType
	{
		MSG_ANY,            // Used only for filtering messages sent on the Bus.
		MSG_LOG,            // string, string
		MSG_ERROR_MESSAGE,  // OK option only 
		MSG_MESSAGE_BOX,    // OK option only (there is no hwnd window so message box result cannot be obtained
		MSG_KEY,            // string, int
		MSG_MOUSE,          // string, int
		MSG_MOUSEPOS,       // float, float
		MSG_MOUSEDELTA,     // float, float
		MSG_SYMBOL,         // string type - HOLD, RELEASE, PRESS, int wideChar ( must be 2 bytes but 4 is still ok )
		MSG_RENDER,
		MSG_AUDIO,
		MSG_PHYS,
		MSG_ENTITY,
		MSG_UI,
		MSG_ENGINE,
		MSG_CONSOLE,
		MSG_ASSET,
		MSG_FILE_LOADED,    // string - full path, FilePathInfo - struct
		MSG_FILE_DELETED,
		MSG_LOAD_DIR,
		MSG_PROFILER_ADD,   // string, string (not needed)
		MSG_PROFILER,       // string, double
		MSG_STRESSTEST,     // isn't meant to be read, only sent
		MSG_FILE_MODIFIED,  // will contain a FilePathInfo object and a new time
		MSG_CREATE_DIR,     // will contain a string with the desired path
		MSG_SCENE
	};

	class Message
	{
	public:
		template <typename T, typename M>
		Message(MessageType Topic, T Event, M Value)
		{
			topic = Topic;
			event = Event;
			value = Value;
		}
		MessageType GetTopic();
		std::any GetEvent();
		std::any GetValue();

	protected:
		MessageType topic;
		std::any event;
		std::any value;
	};

	class Publisher
	{
	public:
		Publisher() {}
		void pSendMessage(Message message);
		void Publish(Channel* channel);
		void Unpublish(Channel* channelPtr);
		void NotifyChannels();

	protected:
		std::vector<Channel*> receivingChannelsList;
		std::queue<Message> messageList;
		std::mutex messageLock;
		std::mutex publishLock;
	};

	class Channel
	{
	public:
		Channel() {}
		void GetNotifyFunction(Message message);
		void AddSubscriber(Subscriber* subscriber);
		void RemoveSubscriber(Subscriber* subscriberPtr);
		void NotifySubs();

	protected:
		std::vector<Subscriber*> receivingSubscribersList;
		std::queue<Message> messageList;
		std::mutex messageLock;
		std::mutex subscribeLock;
	};

	// The Subscribers now use double buffering in order to prevent Message Bus bottlenecks
	// The Message Bus only delivers the message without waiting for the Subscriber to Execute it
	class Subscriber
	{
	public:
		Subscriber() {}

		void AddType(MessageType Type);
		void RemoveType(MessageType Type);
		void Subscribe(Channel* channel);
		void Unsubscribe(Channel* channel);
		void GetNotifyFunction(Message message);
		bool HasType(MessageType Type);
		void SwapBuffers();
		void sUpdate();
		void OnNotify(Message message);
		int GetMessageCount(bool Buffer);

	protected:
		virtual void ExecuteMessage(Message message) {}
		std::vector<MessageType> typeList;
		std::mutex messageLock;
		std::mutex typeLock;
		int buffer = 0;
		std::vector<Message> messageBuffer[2]; // Array of vectors
	};
}