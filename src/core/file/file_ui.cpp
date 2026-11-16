#include "core/file/file.hpp"

#include "core/path.hpp"

std::vector<DataUiElement> FileReader::ReadUiFile(const std::string& uiFilepath) const
{
    static const std::unordered_map<std::string, UiReader> uiReaders = {
        {"scale", &FileReader::ReadScale},
        {"scaleAxis", &FileReader::ReadScaleAxis},
        {"xAnchor", &FileReader::ReadAnchorX},
        {"yAnchor", &FileReader::ReadAnchorY},
        {"xPadding", &FileReader::ReadPaddingX},
        {"yPadding", &FileReader::ReadPaddingY},
        {"text", &FileReader::ReadText},
        {"textsize", &FileReader::ReadTextSize}
    };

    std::optional<std::ifstream> optInput;
    if (CheckFileExtension(uiFilepath, FileExtension::UiFile))
        optInput = OpenFile(std::string(DataDirectory::UiFile) + uiFilepath);
    else if (CheckFileExtension(uiFilepath, FileExtension::UiTemplate))
        optInput = OpenFile(std::string(DataDirectory::UiTemplate) + uiFilepath);

    if (!optInput.has_value())
        return {};
    std::ifstream& input = optInput.value();

    std::vector<DataUiElement> uisData;
    std::string s;
    while (input >> s && s != MAJOR_DELIMITER) {
        if (s == "load") {
            input >> s;
            std::vector<DataUiElement> load = ReadUiFile(s);
            uisData.insert(std::end(uisData), std::begin(load), std::end(load));
            input >> s; // After the load line, the next line must be MAJOR_DELIMITER
            continue;
        }
        DataUiElement data;
        // No verification yet on what is read 
        data.parentKey = s;
        input >> data.key; 
        input >> data.type; // Verification on type will be in UiController
        if (data.type == "uielement" || data.type == "animatedelement") // Not for "textelement"
            input >> data.imagePath;
        
        // Read optional data 
        while (input >> s && s != MAJOR_DELIMITER) {
            if (std::unordered_map<std::string, UiReader>::const_iterator it = uiReaders.find(s); it != uiReaders.end())
                (this->*(it->second))(input, data);
            else {
                Logger::GetInstance().AddLog(Level::L4, "UiParams has no member with this name: " + s); // TODO : Level
                return uisData;
            }
        }
        uisData.push_back(data);
    }
    return uisData;
}

void FileReader::ReadScale(std::ifstream& input, DataUiElement& data) const
{
    input >> data.scale.srcElement;
    data.scale.axis = ReadAxis(input);
    input >> data.scale.amount;
}

void FileReader::ReadScaleAxis(std::ifstream& input, DataUiElement& data) const
{
    data.dstScaleAxis = ReadAxis(input);
}

void FileReader::ReadAnchorX(std::ifstream& input, DataUiElement& data) const
{
    data.xAnchor = ReadAnchor(input);
}

void FileReader::ReadAnchorY(std::ifstream& input, DataUiElement& data) const
{
    data.yAnchor = ReadAnchor(input);
}

void FileReader::ReadPaddingX(std::ifstream& input, DataUiElement& data) const
{
    input >> data.xPadding.srcElement;
    data.xPadding.axis = ReadAxis(input);
    input >> data.xPadding.amount;
}

void FileReader::ReadPaddingY(std::ifstream& input, DataUiElement& data) const
{
    input >> data.yPadding.srcElement;
    data.yPadding.axis = ReadAxis(input);
    input >> data.yPadding.amount;
}

void FileReader::ReadText(std::ifstream& input, DataUiElement& data) const
{
    std::getline(input >> std::ws, data.text); // std::ws discards leading whitespace from input stream // TODO : Should use ReadString ?
}

void FileReader::ReadTextSize(std::ifstream& input, DataUiElement& data) const
{
    data.fontSize = ReadFontSize(input);
}
