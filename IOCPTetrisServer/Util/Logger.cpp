#include "Logger.h"

const int MAX_LOG_SIZE = 1024;

void SBNet::Logger::Debug(const std::string& message) const
{
#ifdef _DEBUG
	category.debug(message);
#endif
}

void SBNet::Logger::Info(const std::string& message) const
{
	if (IsValidLogSize(message))
	{
		category.info(message);
	}
	else
	{
		category.info(message.substr(0, MAX_LOG_SIZE));
	}
}

void SBNet::Logger::Warn(const std::string& message) const
{
	if (IsValidLogSize(message))
	{
		category.warn(message);
	}
	else
	{
		category.warn(message.substr(0, MAX_LOG_SIZE));
	}
}

void SBNet::Logger::Error(const std::string& message) const
{
	if (IsValidLogSize(message))
	{
		category.error(message);
	}
	else
	{
		category.error(message.substr(0, MAX_LOG_SIZE));
	}
}

void SBNet::Logger::Fatal(const std::string& message) const
{
	if (IsValidLogSize(message))
	{
		category.fatal(message);
	}
	else
	{
		category.fatal(message.substr(0, MAX_LOG_SIZE));
	}
}

void SBNet::Logger::Debug(const char* stringFormat, ...) const
{
#ifdef _DEBUG
	va_list args;
	va_start(args, stringFormat);
	const std::string& message = MakeStringFormat(stringFormat, args);
	va_end(args);
	Debug(message);
#endif
}

void SBNet::Logger::Info(const char* stringFormat, ...) const
{
	va_list args;
	va_start(args, stringFormat);
	const std::string& message = MakeStringFormat(stringFormat, args);
	va_end(args);
	Info(message);
}

void SBNet::Logger::Warn(const char* stringFormat, ...) const
{
	va_list args;
	va_start(args, stringFormat);
	const std::string& message = MakeStringFormat(stringFormat, args);
	va_end(args);
	Warn(message);
}

void SBNet::Logger::Error(const char* stringFormat, ...) const
{
	va_list args;
	va_start(args, stringFormat);
	const std::string& message = MakeStringFormat(stringFormat, args);
	va_end(args);
	Error(message);
}

void SBNet::Logger::Fatal(const char* stringFormat, ...) const
{
	va_list args;
	va_start(args, stringFormat);
	const std::string& message = MakeStringFormat(stringFormat, args);
	va_end(args);
	Fatal(message);
}

bool SBNet::Logger::IsValidLogSize(const std::string& message) const
{
	return MAX_LOG_SIZE > message.length();
}

const std::string SBNet::Logger::MakeStringFormat(const char* stringFormat, va_list args) const
{
	char buf[MAX_LOG_SIZE] = { 0, };

	vsprintf_s(buf, MAX_LOG_SIZE - 1, stringFormat, args);

	return buf;
}
