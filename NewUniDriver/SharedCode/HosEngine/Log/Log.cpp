#include "Log.h"

using namespace HOSAuditEngine;
namespace MMobileDriver
{
	namespace Utility
	{

		Log::Log()
		{
		}

		void Log::Debug(const std::string &tag, const std::string &msg)
		{
			#if defined(MY_DEBUG)
			std::cout << std::string::Format("{0}  : {1}",tag,msg) << std::endl;
			#endif
		}

		void Log::Debug1(const std::string &msg)
		{
			#if defined(MY_DEBUG)
			std::cout << std::string::Format("{0}  : {1}","OoO",msg) << std::endl;
			#endif
		}

		void Log::DebugB(const std::string &msg)
		{
			#if defined(MY_DEBUG)
			std::string tag = "Bkg";
			std::cout << std::string::Format("{0}  : {1}",tag,msg) << std::endl;
			Write2LogFile(msg);
			#endif
		}

		void Log::Error(const std::string &tag, const std::string &msg)
		{
			#if defined(MY_DEBUG)
			std::cout << std::string::Format("Error-{0}  : {1}",tag,msg) << std::endl;
			#endif
		}

#if defined(Write2File)
		std::string Log::getPath()
		{
			auto doc = Environment::GetFolderPath(Environment::SpecialFolder::MyDocuments);
			//var log = Path.Combine (doc, "..", "Log");
			auto filename = Path::Combine(doc, "logfile.txt");
			return filename;
		}
#endif

		void Log::ClearLogFile(const std::string &s)
		{
			#if defined(MY_DEBUG)
			auto filename = getPath();
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
			std::string str = DateTime::Now.ToString() + std::string(" : ") + s + std::string("\r\n");
			File::WriteAllText(filename, str);
			#endif
		}

		void Log::Write2LogFile(const std::string &s)
		{
			#if defined(Write2File)
			try
			{
				auto filename = getPath();
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
				std::string str = DateTime::Now.ToString() + std::string(" : ") + s + std::string("\r\n");
				File::AppendAllText(filename, str);
			}
			catch (std::exception &e1)
			{
				// Sharing violation ??? Why??
				//Debug ("LOG", "Write2LogFile EXC:" + ex.Message);
			}
			#endif
		}
	}
}
