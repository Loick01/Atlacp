#include "core/file/file.hpp"

#include <algorithm>

#include "core/path.hpp"

std::optional<std::ifstream> FileReader::OpenFile(const std::string& filepath)
{
    std::ifstream input;
    input.open(filepath);
    if (!input) {
        Logger::GetInstance().AddLog(Level::L4, "Failed to open the file : " + filepath); // TODO : Level
        return std::nullopt;
    }
    return input;
}

std::optional<int> FileReader::GetIntFromStr(const std::string& s) const
{
    try {
        size_t pos;
        const int value = std::stoi(s, &pos);

        if (pos != s.size())
            return std::nullopt;

        return value;
        
    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

std::optional<float> FileReader::GetFloatFromStr(const std::string& s) const
{
    try {
        size_t pos;
        const float value = std::stof(s, &pos);

        if (pos != s.size())
            return std::nullopt;

        return value;
        
    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

std::string FileReader::ReadString(std::ifstream& input) const 
{
    std::string s;
    input >> s;
    std::replace(s.begin(), s.end(), '_', ' ');
    return s;
}

int FileReader::ReadInt(std::ifstream& input) const
{
    std::string s;
    input >> s;
    std::optional<int> optInt = GetIntFromStr(s);
    
    if (!optInt.has_value()) {
        Logger::GetInstance().AddLog(Level::L4, "(FileReader::ReadInt) Failed"); // TODO : Level
        return 0;
    }

    return optInt.value();
}

float FileReader::ReadFloat(std::ifstream& input) const
{
    std::string s;
    input >> s;
    std::optional<float> optFloat = GetFloatFromStr(s);
    
    if (!optFloat.has_value()) {
        Logger::GetInstance().AddLog(Level::L4, "(FileReader::ReadFloat) Failed"); // TODO : Level
        return 0.f;
    }

    return optFloat.value();
}

std::string FileReader::GetFileExtension(const std::string& filepath) const 
{
    size_t pos = filepath.rfind('.');
    if (pos == std::string::npos) {
        Logger::GetInstance().AddLog(Level::L4, "This file has no extension : " + filepath); // TODO : Level
        return "";
    }

    return filepath.substr(pos);
}

bool FileReader::CheckFileExtension(const std::string& filepath, const std::string_view extension) const
{
    return GetFileExtension(filepath) == std::string(extension);
}