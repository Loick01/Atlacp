#include "core/logger.hpp"

#include <fstream>

std::string_view Log::GetLevelLabel() const {
    switch (level) {
        case LogLevel::Level0:
            return "[DEBUG]";
        case LogLevel::Level1:
            return "[INFO]";
        case LogLevel::Level2: 
            return "[WARNING]";
        case LogLevel::Level3:
            return "[ERROR]";
        default:
            Logger::GetInstance().AddLog(LogLevel::Level1, "Unhandled LogLevel used in Log::GetLevelLabel");
            return "[ _ ]";
    }
}

Logger::Logger()
{}

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

const std::vector<Log>& Logger::GetLogs() const
{
    return m_logs;
}

void Logger::WriteLogsInFile() const
{
    std::ofstream output("../execution.log");

    for (const Log& log : m_logs)
        output << log.label << " " << log.message << "\n";

    output.close();
}

void Logger::AddLog(const LogLevel level, const std::string& message)
{
    m_logs.emplace_back(message, level);
}

void Logger::Clear()
{
    m_logs.clear();
}