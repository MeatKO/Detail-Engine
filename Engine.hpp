#pragma once

#include <Windows.h>
#include <chrono>
#include <ctime>
#include <time.h>

#include "ECS.hpp"
#include "Input.hpp"
#include "Log.hpp"
#include "OpenGL.hpp"
#include "Console.hpp"
#include "FileSystem.hpp"

using namespace dMath;

namespace detailEngine
{
	enum SystemThread
	{
		THR_PHYSICS,
		THR_NETWORK,
		THR_AUDIO,
		THR_OUTSOURCE,
		THR_FILESYSTEM,
		THR_LAST
	};

	class Engine : public Publisher, public Subscriber
	{
	public:
		Engine() {}

		void UpdateFileSystem()
		{
			while (threadWork)
			{
				fileSystem->NotifyChannels();
				fileSystem->sUpdate();
				Sleep(50);
			}
		}

		void UpdateThread()
		{
			while (threadWork)
			{
				input->NotifyChannels();
				console->NotifyChannels();

				input->Update(currentTime);
				console->Update(input);

				messageLog->sUpdate();
				console->sUpdate();

				Sleep(10);
			}
		}

		bool Init()
		{
			this->Publish(messageBus);
			input->Publish(messageBus);
			console->Publish(messageBus);
			entityController->Publish(messageBus);
			fileSystem->Publish(messageBus);

			this->Subscribe(messageBus);
			messageLog->Subscribe(messageBus);
			console->Subscribe(messageBus);
			entityController->Subscribe(messageBus);

			this->AddType(MSG_ENGINE);
			this->AddType(MSG_CONSOLE);
			messageLog->AddType(MSG_LOG);
			messageLog->AddType(MSG_KEY);

			threadCount = std::thread::hardware_concurrency();

			pSendMessage(Message(MSG_LOG, std::string("Engine Info"), std::string(std::to_string(threadCount) + " Threads were found.")));

			if (threadCount <= THR_LAST)
				pSendMessage(Message(MSG_LOG, std::string("Engine Info"), std::string("ECS thread count exceeds hardware thread count.")));

			entityController->AddEntity("Entity0");
			entityController->AddComponent("Entity0", Component(CT_POSITION, vec3(0.0f, 0.0f, 0.0f)));

			threadList.resize(THR_LAST);

			if (!renderer->Init("Window", 1200, 900, input))
				pSendMessage(Message(MSG_LOG, std::string("Engine Info"), std::string("Renderer initialization failed.")));

			threadList[THR_OUTSOURCE] = std::move(std::thread(&Engine::UpdateThread, this));
			threadList[THR_FILESYSTEM] = std::move(std::thread(&Engine::UpdateFileSystem, this));

			return true;
		}

		void Update()
		{
			lastTime = currentTime;
			currentTime = (double)clock() / CLOCKS_PER_SEC;
			deltaTime = currentTime - lastTime;
			renderer->time = currentTime;

			this->NotifyChannels();
			entityController->NotifyChannels();
			
			messageBus->NotifySubs();
			
			renderer->Update(entityController, currentTime, deltaTime);

			this->sUpdate();
			entityController->sUpdate();
			renderer->sUpdate();
		}

		bool ShouldClose()
		{
			return shouldClose;
		}

		void Terminate()
		{
			threadWork = false;
			for (std::thread& thr : threadList)
			{
				if (thr.joinable())
				{
				    thr.join();
				}
			}
		}

		void ExecuteMessage(Message message)
		{
			if (message.GetTopic() == MSG_CONSOLE)
			{
				if (std::any_cast<std::string>(message.GetEvent()) == "QUIT")
				{
					shouldClose = true;
				}
			}
		}

	private:
		double currentTime = 0.0f, lastTime = 0.0f, deltaTime = 0.0f;
		unsigned int threadCount = 0;
		bool threadWork = true;
		bool shouldClose = false;

		EntityController* entityController = new EntityController();
		Channel* messageBus = new Channel();
		Log* messageLog = new Log();
		Input* input = new Input();
		OpenGL* renderer = new OpenGL();
		Console* console = new Console();
		FileSystem* fileSystem = new FileSystem();
		std::vector<std::thread> threadList;
	};
}