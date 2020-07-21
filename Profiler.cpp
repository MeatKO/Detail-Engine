#include "Profiler.hpp"

namespace detailEngine
{
	void Profiler::ExecuteMessage(Message message)
	{
		if (message.GetTopic() == MSG_PROFILER)
		{
			std::string profileName = std::any_cast<std::string>(message.GetEvent());
			double newTime = std::any_cast<double>(message.GetValue());

			long int microseconds = newTime * 1000000;

			UpdateProfile(profileName, microseconds);
		}
		else if (message.GetTopic() == MSG_PROFILER_ADD)
		{
			std::string profileName = std::any_cast<std::string>(message.GetEvent());

			AddProfile(profileName);
		}
	}

	void Profiler::UpdateProfile(std::string profileName, long int microseconds)
	{

		if (microseconds > 0)
		{
			for (int i = 0; i < records.size(); ++i)
			{
				if (records[i].name == profileName)
				{
					records[i].microseconds += microseconds;
					if (records[i].minTime > microseconds)
						records[i].minTime = microseconds;
		
					if (records[i].maxTime < microseconds)
						records[i].maxTime = microseconds;
		
					records[i].lastTime = microseconds;

					if (records[i].updates == 1)
						records[i].minTime = microseconds;

					records[i].updates++;
				}
			}
		}
	}

	void Profiler::Update()
	{
		PrintInfo();
	}

	void Profiler::PrintInfo()
	{
		for (ProfileRecord& record : records)
		{
			std::cout << "Profile Name : '" << record.name << "'  Avg. Time : " << record.microseconds / record.updates
				<< "us  Last Time : " << record.lastTime << "us  Min. Time : " << record.minTime << "us  Max. Time : " << record.maxTime << "us\n";
		
		}
	}

	void Profiler::AddProfile(std::string name)
	{
		std::lock_guard<std::mutex> mut(recordMutex);

		records.push_back(ProfileRecord(name));
	}


	// Timer stuff
	// In case of memory leak you probably havent 
	// dont forget to notify channels
	void ProfileTimer::StartTime(std::string recordName, double currentTime)
	{
		TimerRecord* record = GetRecord(recordName);

		if (record)
		{
			record->lastTime = currentTime;
		}
		else
		{
			pSendMessage(Message(MSG_PROFILER_ADD, std::string(recordName), int(0)));
			records.push_back(TimerRecord(recordName, currentTime));
		}
	}

	void ProfileTimer::EndTime(std::string recordName, double currentTime)
	{
		for (TimerRecord& rec : records)
		{
			if (rec.name == recordName)
			{
				pSendMessage(Message(MSG_PROFILER, std::string(recordName), double(currentTime - rec.lastTime)));

				return;
			}
		}

		pSendMessage(Message(MSG_LOG, std::string("Profiler Error"), std::string("Tried to end time for unexisting profile : '" + recordName + "'.")));
	}

	TimerRecord* ProfileTimer::GetRecord(std::string recordName)
	{
		for (TimerRecord& rec : records)
		{
			if (rec.name == recordName)
				return &rec;
		}

		return nullptr;
	}

}
