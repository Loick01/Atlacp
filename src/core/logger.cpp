#include "core/logger.hpp"

#include <fstream>

std::string_view Log::GetLevelLabel() const {
    switch (level) {
        case Level::L0:
            return "[DEBUG]";
        case Level::L1:
            return "[INFO]";
        case Level::L2: 
            return "[WARNING]";
        case Level::L3:
            return "[ERROR]";
        case Level::L4:
            return "[TBD]";
        default:
            Logger::GetInstance().AddLog(Level::L4, "Unhandled Level used in Log::GetLevelLabel");
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

void Logger::AddLog(const Level level, const std::string& message)
{
    m_logs.emplace_back(message, level);
}

void Logger::Clear()
{
    m_logs.clear();
}