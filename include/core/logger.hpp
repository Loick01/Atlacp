#pragma once

#include <string>
#include <string_view>
#include <vector>

enum class LogLevel
{   
    Level0, Level1, Level2, Level3
};

struct Log
{
    const std::string message;
    const LogLevel level;

    const std::string_view label;

    Log(const std::string& m, const LogLevel l):
        message(m), level(l), label(GetLevelLabel())
    {}

    std::string_view GetLevelLabel() const;
};

class Logger // Singleton
{
    private:
        std::vector<Log> m_logs;

    public:
        Logger();

        static Logger& GetInstance();

        const std::vector<Log>& GetLogs() const;
        void WriteLogsInFile() const; // TODO : For now, logs are not filtered according to their LogLevel

        void AddLog(const LogLevel level, const std::string& message);
        void Clear();
};