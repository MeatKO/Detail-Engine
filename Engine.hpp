#pragma once

// Dont forget to :
// Link the publishers and the subscribers to the bus
// use the sUpdate and NotifyChannels
// add types of messages to be received

#include <Windows.h>
#include <chrono>
#include <ctime>
#include <time.h>

#include "ECS.hpp"
#include "VFS.hpp"
#include "Log.hpp"
#include "Input.hpp"
#include "OpenGL.hpp"
#include "Console.hpp"
#include "Profiler.hpp"
#include "DebugSystem.hpp"
#include "WorldManager.hpp"
#include "AssetManager.hpp"
#include "Transformation.hpp"

namespace detailEngine
{
	enum SystemThread
	{
		THR_PHYSICS,
		THR_NETWORK,
		THR_AUDIO,
		THR_OUTSOURCE,
		THR_FILESYSTEM,
		THR_BUS,
		THR_WORLD,
		THR_MISC,
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
				timer->StartTime("FileSystem Loop");

				virtualFileSystem->NotifyChannels();
				virtualFileSystem->sUpdate();
				//virtualFileSystem->Update(entityController, assetManager);
				//debugSystem->Update(input, entityController, assetManager);

				Sleep(10);

				timer->EndTime("FileSystem Loop");
			}
		}

		void UpdateBus()
		{
			while (threadWork)
			{
				messageBus->NotifySubs();
			}
		}

		//void TestFunc()
		//{
		//	while (threadWork)
		//	{
		//		Sleep(1000);
		//		debugSystem->TestMessageBus(4000000);
		//	}
		//}

		void UpdateWorld()
		{
			double wCurrentTime = 0.0f, wLastTime = 0.0f, wDeltaTime = 0.0f;

			while (threadWork)
			{
				wLastTime = wCurrentTime;
				wCurrentTime = (double)clock() / CLOCKS_PER_SEC;
				wDeltaTime = wCurrentTime - wLastTime;

				worldManager->NotifyChannels();
				worldManager->sUpdate();
				worldManager->Update();

				Sleep(1);
			} 
		}

		void UpdateThread()
		{
			while (threadWork)
			{
				timer->StartTime("Update Thread");

				input->NotifyChannels();
				console->NotifyChannels();
				assetManager->NotifyChannels();
				profiler->NotifyChannels();
				debugSystem->NotifyChannels();

				input->Update(currentTime);
				console->Update(input, entityController);
				profiler->Update();

				messageLog->sUpdate();
				console->sUpdate();
				assetManager->sUpdate();
				profiler->sUpdate();
				debugSystem->sUpdate();

				Sleep(1);

				timer->EndTime("Update Thread");
			}
		}

		bool Init()
		{
			timer->StartTime("Engine Init");

			this->Publish(messageBus);
			input->Publish(messageBus);
			console->Publish(messageBus);
			entityController->Publish(messageBus);
			renderer->Publish(messageBus);
			assetManager->Publish(messageBus);
			profiler->Publish(messageBus);
			timer->Publish(messageBus);
			virtualFileSystem->Publish(messageBus);
			debugSystem->Publish(messageBus);
			worldManager->Publish(messageBus);

			this->Subscribe(messageBus);
			messageLog->Subscribe(messageBus);
			console->Subscribe(messageBus);
			entityController->Subscribe(messageBus);
			renderer->Subscribe(messageBus);
			assetManager->Subscribe(messageBus);
			profiler->Subscribe(messageBus);
			virtualFileSystem->Subscribe(messageBus);
			debugSystem->Subscribe(messageBus);
			worldManager->Subscribe(messageBus);

			this->AddType(MSG_ENGINE);
			this->AddType(MSG_CONSOLE);
			messageLog->AddType(MSG_ANY);
			console->AddType(MSG_KEY);
			assetManager->AddType(MSG_ASSET);
			profiler->AddType(MSG_PROFILER);
			profiler->AddType(MSG_PROFILER_ADD);
			worldManager->AddType(MSG_ANY);

			console->AddType(MSG_STRESSTEST);
			assetManager->AddType(MSG_STRESSTEST);
			profiler->AddType(MSG_STRESSTEST);
			profiler->AddType(MSG_STRESSTEST);

			threadCount = std::thread::hardware_concurrency();

			pSendMessage(Message(MSG_LOG, std::string("Engine Info"), std::string(std::to_string(threadCount) + " Threads were found.")));

			if (threadCount <= THR_LAST)
				pSendMessage(Message(MSG_LOG, std::string("Engine Info"), std::string("ECS thread count exceeds hardware thread count.")));

			threadList.resize(THR_LAST);

			if (!renderer->Init("deta:l Engine", 1200, 900, input, 4, 5))
			{
				pSendMessage(Message(MSG_LOG, std::string("Engine Error"), std::string("Renderer initialization failed.")));
				shouldClose = true;
			}

			if (!worldManager->Init(entityController))
			{
				pSendMessage(Message(MSG_LOG, std::string("Engine Error"), std::string("WorldManager initialization failed.")));
				shouldClose = true;
			}

			threadList[THR_OUTSOURCE] = std::move(std::thread(&Engine::UpdateThread, this));
			threadList[THR_FILESYSTEM] = std::move(std::thread(&Engine::UpdateFileSystem, this));
			threadList[THR_BUS] = std::move(std::thread(&Engine::UpdateBus, this));
			threadList[THR_WORLD] = std::move(std::thread(&Engine::UpdateWorld, this));
			//threadList[THR_MISC] = std::move(std::thread(&Engine::TestFunc, this));

			//entityController->AddEntity("Test");
			//assetManager->AddAsset("TestAsset", "FilePath", CAT_AABB);
			//entityController->AddComponent("Test", "TestAsset", CAT_AABB, assetManager);
			worldManager->AddDimension("main");

			//virtualFileSystem->vEnsureVirtualDir("root/kekw/kekw2");
			//virtualFileSystem->PrintTree();

			timer->EndTime("Engine Init");

			return true;
		}

		void Update()
		{
			timer->StartTime("Engine Loop");

			lastTime = currentTime;
			currentTime = (double)clock() / CLOCKS_PER_SEC;
			deltaTime = currentTime - lastTime;
			renderer->time = currentTime;

			this->NotifyChannels();
			entityController->NotifyChannels();
			renderer->NotifyChannels();
			timer->NotifyChannels();
			
			timer->StartTime("Rendering");
			renderer->Update(entityController, assetManager, worldManager, currentTime, deltaTime);
			timer->EndTime("Rendering");

			entityController->Update(assetManager);

			assetManager->Update(entityController, virtualFileSystem);

			this->sUpdate();
			entityController->sUpdate();
			renderer->sUpdate();
			timer->EndTime("Engine Loop");
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

			//entityController->Terminate();
			//assetManager->Terminate();
			//messageBus->Terminate();
			//messageLog->Terminate();
			//input->Terminate();
			//renderer->Terminate();
			//console->Terminate();
			//debugSystem->Terminate();
			//profiler->Terminate();
			//timer->Terminate();
			virtualFileSystem->Terminate(); virtualFileSystem->NotifyChannels();
			worldManager->Terminate(); worldManager->NotifyChannels();

			messageBus->NotifySubs();
			messageLog->sUpdate();
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
		std::vector<std::thread> threadList;

		EntityController* entityController = new EntityController();
		AssetManager* assetManager = new AssetManager();
		Channel* messageBus = new Channel();
		Log* messageLog = new Log();
		Input* input = new Input();
		OpenGL* renderer = new OpenGL();
		Console* console = new Console();
		DebugSystem* debugSystem = new DebugSystem();
		Profiler* profiler = new Profiler();
		ProfileTimer* timer = new ProfileTimer();
		VirtualFileSystem* virtualFileSystem = new VirtualFileSystem();
		WorldManager* worldManager = new WorldManager();
	};
}