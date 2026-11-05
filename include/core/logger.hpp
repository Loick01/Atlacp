#pragma once

#include <string>
#include <string_view>
#include <vector>

enum class Level
{   
    L0, L1, L2, L3, L4
};

struct Log
{
    const std::string message;
    const Level level;

    const std::string_view label;

    Log(const std::string& m, const Level l):
        message(m), level(l), label(GetLevelLabel())
    {}

    std::string_view GetLevelLabel() const;
};

class Logger
{
    private:
        std::vector<Log> m_logs;

    public:
        Logger();

        static Logger& GetInstance();

        const std::vector<Log>& GetLogs() const;
        void WriteLogsInFile() const; // TODO : For now, logs are not filtered according to their Level

        void AddLog(const Level level, const std::string& message);
        void Clear();
};