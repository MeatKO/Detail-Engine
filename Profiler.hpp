#pragma once

// Will hold a list of name & number pairs to show execution time for 
// Dont forget to attach it to the message bus and add topics

#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <iterator>
#include <bitset>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include "ECS.hpp"

namespace detailEngine
{
	struct ProfileRecord
	{
		ProfileRecord(std::string Name) : name(Name) {}
		std::string name;
		unsigned long int updates = 1;
		unsigned long int microseconds = 0;
		long int lastTime = 0;
		long int minTime = 0;
		long int maxTime = 0;
	};

	struct TimerRecord
	{
		TimerRecord(std::string Name, double currentTime) : name(Name), lastTime(currentTime) {}
		std::string name;
		double lastTime = 0.0f;
	};

	// will be on a different thread, calculating stuff 1ns and 0s serious business
	class Profiler : public Publisher, public Subscriber
	{
	public:
		Profiler() {}

		void ExecuteMessage(Message message);
		
		void UpdateProfile(std::string profileName, long int microseconds); // must be multiplied by 1.000.000 to get nanoseconds

		void Update();

		void PrintInfo();

		void AddProfile(std::string name);
		

	private:
		std::mutex recordMutex;
		std::vector<ProfileRecord> records;
	};

	// will wrap around functions ( not really a wrapper ) and will start and end their individual profiles,
	// then send a message on the bus with the name and delta time
	class ProfileTimer : public Publisher
	{
	public:
		ProfileTimer() {}

		void StartTime(std::string recordName, double currentTime); // will automatically add a new profile with the name... cant have duplicate names
		void EndTime(std::string recordName, double currentTime); // will delete the record once it has been ended
		TimerRecord* GetRecord(std::string recordName);

	private:
		std::vector<TimerRecord> records;
	};
}