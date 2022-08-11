#pragma once

#include <string>
#include <cstdarg>
#include "log4cpp/PropertyConfigurator.hh"
#include "log4cpp/Category.hh"

namespace SBNet
{
	class Logger
	{
	public:
		static void LoadConfig()
		{
			std::string initFileName = "log4cpp.properties";
			log4cpp::PropertyConfigurator::configure(initFileName);
		}

		static void Shutdown()
		{
			log4cpp::Category::shutdown();
		}

		Logger(const std::string& loggerName)
			: category(log4cpp::Category::getInstance(loggerName))
		{

		}

		Logger(const char* loggerName)
			: Logger(std::string(loggerName))
		{

		}

		void Debug(const std::string& message) const;

		void Info(const std::string& message) const;

		void Warn(const std::string& message) const;

		void Error(const std::string& message) const;

		void Fatal(const std::string& message) const;

		void Debug(const char* stringFormat, ...) const;

		void Info(const char* stringFormat, ...) const;

		void Warn(const char* stringFormat, ...) const;

		void Error(const char* stringFormat, ...) const;

		void Fatal(const char* stringFormat, ...) const;
	private:
		bool IsValidLogSize(const std::string& message) const;
		const std::string MakeStringFormat(const char* stringFormat, va_list args) const;
		log4cpp::Category& category;
	};

}
