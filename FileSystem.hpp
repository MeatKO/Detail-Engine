#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <bitset>
#include <ctime>
#include "ECS.hpp"
#include "glModel.hpp"

namespace detailEngine
{
	long int unix_timestamp()
	{
		time_t t = std::time(0);
		long int now = static_cast<long int> (t);
		return now;
	}

	class File
	{
	public:
		File() {}
	};

	class Pack
	{
	public:
		Pack() {}
	};

	class FileSystem : public Publisher, public Subscriber
	{
	public:
		FileSystem() {}


		void ExecuteMessage(Message message)
		{

		}
	};
}