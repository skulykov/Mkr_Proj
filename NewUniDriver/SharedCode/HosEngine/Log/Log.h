#pragma once

#include <string>
#include <iostream>
#include <stdexcept>

#define Write2File
#define MY_DEBUG
using namespace HOSAuditEngine;

namespace MMobileDriver
{
	namespace Utility
	{
		class Log
		{
		public:
			Log();
			static void Debug(const std::string &tag, const std::string &msg);
			static void Debug1(const std::string &msg);
			static void DebugB(const std::string &msg);

			static void Error(const std::string &tag, const std::string &msg);

			#if defined(Write2File)
		private:
			static std::string getPath();
			#endif
		public:
			static void ClearLogFile(const std::string &s);
			static void Write2LogFile(const std::string &s);

		};
	}
}

