#include "pch.h"
#include "Log.h"

// Function called on logging to print/write log information.
std::function<void(const char *, const char *, bool)> ps::log::OnPrint;
std::function<void(const ps::LogType, const char *, bool)> ps::log::OnPrint2;
// The current log buffer for characters.
std::unique_ptr<char[]> ps::log::LogBuffer;
// The current log stream.
std::ofstream ps::log::LogStream;
// The current log mutex.
std::mutex ps::log::LogMutex;
// The current log types.
ps::LogType ps::log::LogTypes;

void ps::log::Init(const std::string &logFile)
{
    LogStream.exceptions(std::ios::badbit | std::ios::failbit);
    LogStream.open(logFile);
}

void ps::log::EnableLogType(LogType logType)
{
    auto current = (int)LogTypes | (int)logType;
    LogTypes = (ps::LogType)current;
}

void ps::log::DisableLogType(LogType logType)
{
    auto current = (int)LogTypes ^ (int)logType;
    LogTypes = (ps::LogType)current;
}

void ps::log::SetOnPrint(std::function<void(const char *, const char *, bool)> onPrint)
{
    OnPrint = onPrint;
}

void ps::log::SetOnPrint2(std::function<void(const LogType logType, const char *, bool b)> onPrint2)
{
    OnPrint2 = onPrint2;
}