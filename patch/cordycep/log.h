#pragma once
#include "LogType.h"

namespace ps
{
    namespace log
    {
        // Function called on logging to print/write log information.
        extern std::function<void(const char *, const char *, bool b)> OnPrint;
        extern std::function<void(const LogType logType, const char *, bool b)> OnPrint2;
        // The current log buffer for characters.
        extern std::unique_ptr<char[]> LogBuffer;
        // The current log stream.
        extern std::ofstream LogStream;
        // The current log mutex.
        extern std::mutex LogMutex;
        // The current log types.
        extern LogType LogTypes;

        // Initializes the logger.
        void Init(const std::string &logFile);
        // Enables the provided log type.
        void EnableLogType(LogType logType);
        // Disables the provided log type.
        void DisableLogType(LogType logType);
        // Sets the print callback
        void SetOnPrint(std::function<void(const char *, const char *, bool)> onPrint);
        // Sets the print callback 2
        void SetOnPrint2(std::function<void(const LogType logType, const char *, bool b)> onPrint2);
        // Outputs the message to the log file.
        template <typename... Args>
        void Log(const LogType logType, const char *value, Args... args)
        {
            auto current = (int)LogTypes;
            auto toCheck = (int)logType;

            if ((current & toCheck) == 0)
                return;
            if (LogBuffer == nullptr)
                LogBuffer = std::make_unique<char[]>(32768);

            std::lock_guard<std::mutex> guardDog(LogMutex);

            auto lt = tm();
            auto ln = std::time(nullptr);

            auto error = localtime_s(&lt, &ln);
            auto to = std::put_time(&lt, "%d-%m-%Y %H-%M-%S");

            std::stringstream ss;
            ss << std::put_time(&lt, "%d-%m-%Y %H-%M-%S");

            auto timeStr = ss.str();

            LogStream << "[ ";
            LogStream << timeStr << std::setw(24 - timeStr.size());
            LogStream << " ]";
            LogStream << " ";

            size_t size = (size_t)std::snprintf(nullptr, 0, value, args...) + 1;
            std::unique_ptr<char[]> buf(new char[size]);
            std::snprintf(buf.get(), size, value, args...);

            LogStream << buf.get() << "\n";
            LogStream.flush();

            if (OnPrint2)
            {
                OnPrint2(logType, buf.get(), true);
            }
        }
        // Outputs the message to the log file.
        template <typename... Args>
        void Print(const char *header, const char *value, Args... args)
        {
            auto current = (int)LogTypes;
            auto toCheck = (int)LogType::Normal;

            if ((current & toCheck) == 0)
                return;
            if (LogBuffer == nullptr)
                LogBuffer = std::make_unique<char[]>(32768);

            std::lock_guard<std::mutex> guardDog(LogMutex);

            auto lt = tm();
            auto ln = std::time(nullptr);

            auto error = localtime_s(&lt, &ln);
            auto to = std::put_time(&lt, "%d-%m-%Y %H-%M-%S");

            std::stringstream ss;
            ss << std::put_time(&lt, "%d-%m-%Y %H-%M-%S");

            auto timeStr = ss.str();

            LogStream << "[ ";
            LogStream << timeStr << std::setw(24 - timeStr.size());
            LogStream << " ]";
            LogStream << " ";

            size_t size = (size_t)std::snprintf(nullptr, 0, value, args...) + 1;
            std::unique_ptr<char[]> buf(new char[size]);
            std::snprintf(buf.get(), size, value, args...);

            LogStream << buf.get() << "\n";
            LogStream.flush();

            if (OnPrint)
            {
                OnPrint(header, buf.get(), true);
            }
        }
        // Outputs the message to the log file.
        template <typename... Args>
        std::string Input(const char *header, const char *value, Args... args)
        {
            auto current = (int)LogTypes;
            auto toCheck = (int)LogType::Normal;

            if ((current & toCheck) == 0)
                return "";
            if (LogBuffer == nullptr)
                LogBuffer = std::make_unique<char[]>(32768);

            std::lock_guard<std::mutex> guardDog(LogMutex);

            auto lt = tm();
            auto ln = std::time(nullptr);

            auto error = localtime_s(&lt, &ln);
            auto to = std::put_time(&lt, "%d-%m-%Y %H-%M-%S");

            std::stringstream ss;
            ss << std::put_time(&lt, "%d-%m-%Y %H-%M-%S");

            auto timeStr = ss.str();

            LogStream << "[ ";
            LogStream << timeStr << std::setw(24 - timeStr.size());
            LogStream << " ]";
            LogStream << " ";

            size_t size = (size_t)std::snprintf(nullptr, 0, value, args...) + 1;
            std::unique_ptr<char[]> buf(new char[size]);
            std::snprintf(buf.get(), size, value, args...);

            LogStream << buf.get() << "\n";
            LogStream.flush();

            if (OnPrint)
            {
                OnPrint(header, buf.get(), false);
            }

            std::string result;
            std::getline(std::cin, result);
            return result;
        }
    }
}
