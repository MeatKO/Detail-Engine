#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include "ECS.hpp"
#include "glModel.hpp"

namespace detailEngine
{
	class File
	{
	public:
		File(std::string Name) : name(Name) {}
		std::string name = "";
		std::string contents = "";
	};

	class Pack
	{
	public:
		Pack(std::string Name) : name(Name) {}
		std::string name = "";
		std::vector<std::string> filenamesList;
		std::vector<File> filesList;
	};

	class FileSystem : public Publisher, public Subscriber
	{
	public:
		FileSystem() {}
		
		bool OpenPack(std::string packName)
		{
			std::string packPath = "detail/" + packName + ".nbf";
			std::ifstream file(packPath);
			
			if (!file.is_open())
			{
				pSendMessage(Message(MSG_LOG, std::string("FileSystem Error"), std::string("Couldn't open pack '" + packName + ".nbf'")));
				return false;
			}

			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string inStr = buffer.str();
			std::cout << inStr << std::endl;
			//std::string curline;
			//while (std::getline(file, curline))
			//{
			//	
			//	if (firstToken(curline) == "o" || firstToken(curline) == "g" || curline[0] == 'g')
			//	{
			//
			//	}
			//}
			

			file.close();
		}

		//inline std::string firstToken(const std::string& in)
		//{
		//	if (!in.empty())
		//	{
		//		size_t token_start = in.find_first_not_of(" \t");
		//		size_t token_end = in.find_first_of(" \t", token_start);
		//		if (token_start != std::string::npos && token_end != std::string::npos)
		//		{
		//			return in.substr(token_start, token_end - token_start);
		//		}
		//		else if (token_start != std::string::npos)
		//		{
		//			return in.substr(token_start);
		//		}
		//	}
		//	return "";
		//}

		void ExecuteMessage(Message message)
		{

		}

		std::vector<Pack> packsList;
		std::string hash = "1be5c61c";
	};
}