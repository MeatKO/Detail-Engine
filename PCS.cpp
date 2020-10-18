#include "PCS.hpp"

namespace detailEngine
{
	// Message
    MessageType Message::GetTopic()
	{
		return topic;
	}
	std::any Message::GetEvent()
	{
		return event;
	}
	std::any Message::GetValue()
	{
		return value;
	}

	// Publisher
	void Publisher::pSendMessage(Message message)
	{
		std::lock_guard<std::mutex> mut(messageLock);
		messageList.push(message);
	}

	void Publisher::Publish(Channel* channel)
	{
		std::lock_guard<std::mutex> mut(publishLock);
		receivingChannelsList.push_back(channel);
	}

	void Publisher::Unpublish(Channel* channelPtr)
	{
		std::lock_guard<std::mutex> mut(publishLock);
		for (Channel* searchedPtr : receivingChannelsList)
		{
			for (unsigned int loop = 0; loop < receivingChannelsList.size(); loop++)
			{
				if (searchedPtr == channelPtr)
				{
					receivingChannelsList.erase(receivingChannelsList.begin() + loop);
				}
			}
		}
	}
	void Publisher::NotifyChannels()
	{
		std::lock_guard<std::mutex> mut(messageLock);
		while (!messageList.empty())
		{
			for (Channel* iter : receivingChannelsList)
			{
				iter->GetNotifyFunction(messageList.front());
			}

			messageList.pop();
		}
	}

	// Channel
	void Channel::GetNotifyFunction(Message message)
	{
		std::lock_guard<std::mutex> mut(messageLock);
		messageList.push(message);
	}
	void Channel::AddSubscriber(Subscriber* subscriber)
	{
		std::lock_guard<std::mutex> mut(subscribeLock);
		receivingSubscribersList.push_back(subscriber);
	}
	void Channel::RemoveSubscriber(Subscriber* subscriberPtr)
	{
		std::lock_guard<std::mutex> mut(subscribeLock);
		for (Subscriber* searchedPtr : receivingSubscribersList)
		{
			for (unsigned int loop = 0; loop < receivingSubscribersList.size(); loop++)
			{
				if (searchedPtr == subscriberPtr)
				{
					receivingSubscribersList.erase(receivingSubscribersList.begin() + loop);
				}
			}
		}
	}
	void Channel::NotifySubs()
	{
		std::lock_guard<std::mutex> mut(messageLock);
		while (!messageList.empty())
		{
			for (Subscriber* iterator : receivingSubscribersList)
			{
				if (iterator->HasType(messageList.front().GetTopic()) || iterator->HasType(MSG_ANY))
				{
					iterator->GetNotifyFunction(messageList.front());
				}
			}

			messageList.pop();
		}
	}

	// Subscriber
	void Subscriber::AddType(MessageType Type)
	{
		std::lock_guard<std::mutex> mut(typeLock);
		if (!HasType(Type))
		{
			typeList.push_back(Type);
		}
	}
	void Subscriber::RemoveType(MessageType Type)
	{
		std::lock_guard<std::mutex> mut(typeLock);
		for (unsigned int loop = 0; loop < typeList.size(); loop++)
		{
			if (Type == typeList[loop])
			{
				typeList.erase(typeList.begin() + loop);
			}
		}
	}
	void Subscriber::Subscribe(Channel* channel)
	{
		channel->AddSubscriber(this);
	}
	void Subscriber::Unsubscribe(Channel* channel)
	{
		channel->RemoveSubscriber(this);
	}
	void Subscriber::GetNotifyFunction(Message message)
	{
		std::lock_guard<std::mutex> mut(messageLock);
		OnNotify(message);
	}
	bool Subscriber::HasType(MessageType Type)
	{
		for (MessageType type : typeList)
		{
			if (Type == type)
			{
				return true;
			}
		}
		return false;
	}
	void Subscriber::SwapBuffers()
	{
		std::lock_guard<std::mutex> mut(messageLock);
		buffer = !buffer;
	}
	// The non-blocking message execution only works if its on a different thread
	void Subscriber::sUpdate()
	{
		SwapBuffers();
		for (Message& msg : messageBuffer[!buffer])
		{
			ExecuteMessage(msg);
		}
		messageBuffer[!buffer].clear();
	}
	void Subscriber::OnNotify(Message message)
	{
		messageBuffer[buffer].push_back(message);
	}
	int Subscriber::GetMessageCount(bool Buffer)
	{
		return messageBuffer[Buffer].size();
	}
}
