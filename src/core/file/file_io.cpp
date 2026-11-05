#include "core/file/file.hpp"

#include <algorithm>

#include "core/path.hpp"

std::ifstream FileReader::OpenFile(const std::string& filepath)
{
    std::ifstream input;
    input.open(filepath);
    if (!input)
        Logger::GetInstance().AddLog(Level::L4, "Failed to open the file : " + filepath);

    return input;
}

std::string FileReader::ReadString(std::ifstream& input) const 
{
    std::string s;
    input >> s;
    std::replace(s.begin(), s.end(), '_', ' ');
    return s;
}

std::string FileReader::GetFileExtension(const std::string& filepath) const 
{
    size_t pos = filepath.rfind('.');
    if (pos == std::string::npos) {
        Logger::GetInstance().AddLog(Level::L4, "This file has no extension : " + filepath);
        return "";
    }

    return filepath.substr(pos);
}

bool FileReader::IsBaseUiFile(const std::string& filepath) const
{
    return GetFileExtension(filepath) == std::string(FileExtension::UiFile);
}

bool FileReader::IsTemplateUiFile(const std::string& filepath) const
{
    return GetFileExtension(filepath) == std::string(FileExtension::UiTemplate);
}