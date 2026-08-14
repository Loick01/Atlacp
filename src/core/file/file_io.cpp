#include "core/file/file.hpp"

#include <algorithm>

#include "core/path.hpp"

std::ifstream FileReader::OpenFile(const std::string& filepath)
{
    std::ifstream input;
    input.open(filepath);
    if (!input) throw std::runtime_error("Can't open this file : " + filepath);
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
    if (pos == std::string::npos)
        throw std::runtime_error("This UI file has no extension : " + filepath);
    return filepath.substr(pos);
}

bool FileReader::IsBaseUiFile(const std::string& filepath) const
{
    return GetFileExtension(filepath) == FileExtension::UiFile;
}

bool FileReader::IsTemplateUiFile(const std::string& filepath) const
{
    return GetFileExtension(filepath) == FileExtension::UiTemplate;
}